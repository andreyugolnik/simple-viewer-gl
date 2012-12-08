////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// date: 21.09.2012-Aug-2011
// changed: 21.09.2012-Aug-2011
// version: 0.0.0.10
//
////////////////////////////////////////////////

#include "formatxwd.h"
#include <string.h>

CFormatXwd::CFormatXwd(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

CFormatXwd::~CFormatXwd()
{
}

bool CFormatXwd::Load(const char* filename, unsigned subImage)
{
    if(openFile(filename) == false)
    {
        return false;
    }

	X11WINDOWDUMP header;

	if(sizeof(header) != fread(&header, 1, sizeof(header), m_file))
	{
		std::cout << "Can't read XWD header" << std::endl;
		reset();
		return false;
	}

    swap_long((uint8_t*)&header, sizeof(header));

    X11COLORMAP* colors = new X11COLORMAP[header.ColorMapEntries];
    std::cout << "Colormap size: " << header.ColorMapEntries << std::endl;
    if(header.ColorMapEntries != fread(colors, sizeof(X11COLORMAP), header.ColorMapEntries, m_file))
    {
        delete[] colors;
		std::cout << "Can't read colormap" << std::endl;
		reset();
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

    fclose(m_file);

    return true;
}

