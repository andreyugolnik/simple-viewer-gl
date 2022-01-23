/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#if defined(TIFF_SUPPORT)

#include "formattiff.h"
#include "cms/cms.h"
#include "common/bitmap_description.h"
#include "common/file.h"

#include <cstring>
#include <stdarg.h>
#include <tiffio.h>

namespace
{
    void ErrorHandler(const char*, const char*, va_list)
    {
        // ::printf("(EE) \n");
    }

    void WarningHandler(const char*, const char*, va_list)
    {
        // ::printf("(WW) \n");
    }
} // namespace

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
            struct Icc
            {
                bool hasEmbeded() const
                {
                    return profileSize && profile != nullptr;
                }

                uint32_t profileSize = 0;
                void* profile = nullptr;

                bool hasTables() const
                {
                    return chr != nullptr && wp != nullptr && gmr != nullptr && gmg != nullptr && gmb != nullptr;
                }

                float* chr = nullptr;
                float* wp = nullptr;
                uint16_t* gmr = nullptr;
                uint16_t* gmg = nullptr;
                uint16_t* gmb = nullptr;
            };

            Icc icc;

            if (TIFFGetField(tif, TIFFTAG_ICCPROFILE, &icc.profileSize, &icc.profile) == 0)
            {
                if (TIFFGetField(tif, TIFFTAG_PRIMARYCHROMATICITIES, &icc.chr))
                {
                    if (TIFFGetField(tif, TIFFTAG_WHITEPOINT, &icc.wp))
                    {
                        TIFFGetFieldDefaulted(tif, TIFFTAG_TRANSFERFUNCTION, &icc.gmr, &icc.gmg, &icc.gmb);
                    }
                }
            }

            TIFFRGBAImage img;
            char emsg[1024];
            if (TIFFRGBAImageBegin(&img, tif, 0, emsg) != 0)
            {
                desc.width = img.width;
                desc.height = img.height;
                desc.pitch = desc.width * sizeof(uint32_t);
                desc.bitmap.resize(desc.pitch * desc.height);
                desc.bpp = 32;
                desc.bppImage = img.bitspersample * img.samplesperpixel;
                desc.format = GL_RGBA;

                // set desired orientation
                img.req_orientation = ORIENTATION_TOPLEFT;

                m_formatName = "tiff";

                auto bitmap = desc.bitmap.data();
                result = TIFFRGBAImageGet(&img, (uint32_t*)bitmap, desc.width, desc.height) != 0;

                if (result)
                {
                    m_formatName = "tiff/icc";

                    if (icc.hasEmbeded())
                    {
                        applyIccProfile(desc, icc.profile, icc.profileSize);
                    }
                    else if (icc.hasTables())
                    {
                        applyIccProfile(desc, icc.chr, icc.wp, icc.gmr, icc.gmg, icc.gmb);
                    }
                }

                TIFFRGBAImageEnd(&img);
            }
            else
            {
                ::printf("(EE) Error: '%s'\n", emsg);
            }
        }

        TIFFClose(tif);
    }

    return result;
}

#endif
