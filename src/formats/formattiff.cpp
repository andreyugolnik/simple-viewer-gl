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

#include <cstring>
#include <tiffio.h>

cFormatTiff::cFormatTiff(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatTiff::~cFormatTiff()
{
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

    auto tif = TIFFOpen(m_filename.c_str(), "r");
    if (tif != nullptr)
    {
        // read count of pages in image
        desc.images = TIFFNumberOfDirectories(tif);
        desc.current = std::min(current, desc.images - 1);

        // set desired page
        if (TIFFSetDirectory(tif, desc.current) != 0)
        {
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

                result = TIFFRGBAImageGet(&img, (uint32*)desc.bitmap.data(), desc.width, desc.height) != 0;
                TIFFRGBAImageEnd(&img);
            }
        }

        TIFFClose(tif);
    }

    m_formatName = "tiff";

    return result;
}
