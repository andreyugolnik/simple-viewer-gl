/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formattiff.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"
#include "../cms/cms.h"

#include <cstring>
#include <tiffio.h>
#include <stdarg.h>

namespace
{

    void locateICCProfile(TIFF* tif, cCMS& cms)
    {
        unsigned iccProfileSize = 0;
        void* iccProfile = nullptr;
        if (TIFFGetField(tif, TIFFTAG_ICCPROFILE, &iccProfileSize, &iccProfile))
        {
            cms.createTransform(iccProfile, iccProfileSize, cCMS::Pixel::Rgba);
        }
        else
        {
            float* chr;
            if (TIFFGetField(tif, TIFFTAG_PRIMARYCHROMATICITIES, &chr))
            {
                float* wp;
                if (TIFFGetField(tif, TIFFTAG_WHITEPOINT, &wp))
                {
                    unsigned short* gmr;
                    unsigned short* gmg;
                    unsigned short* gmb;
                    TIFFGetFieldDefaulted(tif, TIFFTAG_TRANSFERFUNCTION, &gmr, &gmg, &gmb);

                    cms.createTransform(chr, wp, gmr, gmg, gmb, cCMS::Pixel::Rgba);
                }
            }
        }
    }

    void ErrorHandler(const char*, const char*, va_list)
    {
        // ::printf("(EE) \n");
    }

    void WarningHandler(const char*, const char*, va_list)
    {
        // ::printf("(WW) \n");
    }

}

cFormatTiff::cFormatTiff(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatTiff::~cFormatTiff()
{
}

bool cFormatTiff::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, sizeof(uint32_t)))
    {
        return false;
    }

    const auto h = buffer.data();
    const uint8_t le[4] = { 0x49, 0x49, 0x2A, 0x00 };
    const uint8_t be[4] = { 0x4D, 0x4D, 0x00, 0x2A };
    return !::memcmp(h, le, sizeof(le)) || !::memcmp(h, be, sizeof(be));
}

bool cFormatTiff::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    m_filename = filename;
    return load(0, desc);
}

bool cFormatTiff::LoadSubImageImpl(unsigned current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool cFormatTiff::load(unsigned current, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(m_filename.c_str()))
    {
        return false;
    }

    desc.size = file.getSize();

    file.close();

    bool result = false;

    TIFFSetErrorHandler(ErrorHandler);
    TIFFSetWarningHandler(WarningHandler);

    auto tif = TIFFOpen(m_filename.c_str(), "r");
    if (tif != nullptr)
    {
        // read count of pages in image
        desc.images = TIFFNumberOfDirectories(tif);
        desc.current = std::min(current, desc.images - 1);

        // set desired page
        if (TIFFSetDirectory(tif, desc.current) != 0)
        {
            locateICCProfile(tif, m_cms);

            TIFFRGBAImage img;
            if (TIFFRGBAImageBegin(&img, tif, 0, nullptr) != 0)
            {
                desc.width = img.width;
                desc.height = img.height;
                desc.pitch = desc.width * sizeof(uint32);
                desc.bitmap.resize(desc.pitch * desc.height);
                desc.bpp = 32;
                desc.bppImage = img.bitspersample * img.samplesperpixel;
                desc.format = GL_RGBA;

                // set desired orientation
                img.req_orientation = ORIENTATION_TOPLEFT;

                m_formatName = m_cms.hasTransform() ? "tiff/icc" : "tiff";

                auto bitmap = desc.bitmap.data();
                result = TIFFRGBAImageGet(&img, (uint32*)bitmap, desc.width, desc.height) != 0;

                if (result && m_cms.hasTransform())
                {
                    for (unsigned i = 0; i < desc.height; i++)
                    {
                        m_cms.doTransform(bitmap, bitmap, desc.width);
                    }
                }

                TIFFRGBAImageEnd(&img);
            }
        }

        m_cms.destroyTransform();

        TIFFClose(tif);
    }

    return result;
}
