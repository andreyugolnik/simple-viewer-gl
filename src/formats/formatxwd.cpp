/**********************************************\
*
*  Andrey A. Ugolnik
*  Tiny Orange
*  http://www.tinyorange.com
*  andrey@ugolnik.info
*
*  created: 21.09.2012
*
\**********************************************/

#include "formatxwd.h"
#include <string.h>

CFormatXwd::CFormatXwd(Callback callback, const char* lib, const char* name)
    : CFormat(callback, lib, name)
{
}

CFormatXwd::~CFormatXwd()
{
}

bool CFormatXwd::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    X11WINDOWDUMP header;

    if(sizeof(header) != file.read(&header, sizeof(header)))
    {
        std::cout << "Can't read XWD header" << std::endl;
        return false;
    }

    swap_long((uint8_t*)&header, sizeof(header));

    X11COLORMAP* colors = new X11COLORMAP[header.ColorMapEntries];
    std::cout << "Colormap size: " << header.ColorMapEntries << std::endl;
    if(sizeof(X11COLORMAP) * header.ColorMapEntries != file.read(colors, sizeof(X11COLORMAP) * header.ColorMapEntries))
    {
        delete[] colors;
        std::cout << "Can't read colormap" << std::endl;
        return false;
    }

    m_width		= header.PixmapWidth;
    m_height	= header.PixmapHeight;
    m_bpp		= header.BitsPerPixel;
    m_bppImage	= header.BitsPerPixel;
    m_pitch		= header.BytesPerLine;
    m_bitmap.resize(m_pitch * m_height);
    m_format	= GL_RGB;

    //uint8_t* p = m_bitmap;
    //for(int i = 0; i < ; i++)
    //{
        //int idx = ;
        //p[0] = colors[idx]->Red;
        //p[1] = colors[idx]->Green;
        //p[2] = colors[idx]->Blue;
        //p += 3;
    //}

    delete[] colors;

    return true;
}

