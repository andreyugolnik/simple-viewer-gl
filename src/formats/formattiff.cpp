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

CFormatTiff::CFormatTiff(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

CFormatTiff::~CFormatTiff()
{
}

bool CFormatTiff::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    m_filename = filename;
    return load(0, desc);
}

bool CFormatTiff::LoadSubImageImpl(unsigned current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool CFormatTiff::load(unsigned current, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(m_filename.c_str()))
    {
        return false;
    }

    desc.size = file.getSize();

    file.close();

    bool result = false;

    TIFF* tif = TIFFOpen(m_filename.c_str(), "r");
    if (tif != 0)
    {
        // read count of pages in image
        desc.images = TIFFNumberOfDirectories(tif);
        desc.current = std::min(current, desc.images - 1);

        // set desired page
        if (TIFFSetDirectory(tif, desc.current) != 0)
        {
            TIFFRGBAImage img;
            if (TIFFRGBAImageBegin(&img, tif, 0, NULL) != 0)
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

                if (TIFFRGBAImageGet(&img, (uint32*)&desc.bitmap[0], desc.width, desc.height) != 0)
                {
                    result = true;
                }
                TIFFRGBAImageEnd(&img);
            }
        }
    }

    if (tif != 0)
    {
        TIFFClose(tif);
    }

    return result;
}
