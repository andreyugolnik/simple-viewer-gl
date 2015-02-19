/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formattiff.h"
#include "file.h"

#include <cstring>
#include <tiffio.h>

CFormatTiff::CFormatTiff(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

CFormatTiff::~CFormatTiff()
{
}

bool CFormatTiff::Load(const char* filename, unsigned subImage)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    file.close();

    bool result = false;

    TIFF* tif = TIFFOpen(filename, "r");
    if(tif != 0)
    {
        // read count of pages in image
        m_subCount = TIFFNumberOfDirectories(tif);
        m_subImage = std::min(subImage, m_subCount - 1);

        // set desired page
        if(TIFFSetDirectory(tif, m_subImage) != 0)
        {
            TIFFRGBAImage img;
            if(TIFFRGBAImageBegin(&img, tif, 0, NULL) != 0)
            {
                m_width = img.width;
                m_height = img.height;
                m_pitch = m_width * sizeof(uint32);
                m_bitmap.resize(m_pitch * m_height);
                m_bpp = 32;
                m_bppImage = img.bitspersample * img.samplesperpixel;
                m_format = GL_RGBA;

                // set desired orientation
                img.req_orientation = ORIENTATION_TOPLEFT;

                if(TIFFRGBAImageGet(&img, (uint32*)&m_bitmap[0], m_width, m_height) != 0)
                {
                    result = true;
                }
                TIFFRGBAImageEnd(&img);
            }
        }
    }

    if(tif != 0)
    {
        TIFFClose(tif);
    }

    return result;
}

