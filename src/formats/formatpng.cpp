////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 26-Aug-2010
// changed: 21.09.2012-Aug-2011
// version: 0.0.0.30
//
////////////////////////////////////////////////

#include "formatpng.h"
#include <png.h>
#include <string.h>
#include <iostream>

CFormatPng::CFormatPng(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

CFormatPng::~CFormatPng()
{
}

bool CFormatPng::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    png_byte header[8];	// 8 is the maximum size that can be checked
    size_t size = file.read(header, 8);
    if(size != 8 || png_sig_cmp(header, 0, 8) != 0)
    {
        std::cout << "File " << filename << " is not recognized as a PNG file" << std::endl;
        reset();
        return false;
    }

    // initialize stuff
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(png == 0)
    {
        std::cout << "png_create_read_struct failed" << std::endl;
        reset();
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if(info == 0)
    {
        std::cout << "png_create_info_struct failed" << std::endl;
        reset();
        return false;
    }

    if(setjmp(png_jmpbuf(png)) != 0)
    {
        std::cout << "Error during init_io" << std::endl;
        reset();
        return false;
    }

    png_init_io(png, file.getHandle());
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);

    // get real bits per pixel
    m_bppImage = png_get_bit_depth(png, info) * png_get_channels(png, info);

    uint8_t color_type = png_get_color_type(png, info);
    if(color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }

    if(png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
    }
    if(png_get_bit_depth(png, info) == 16)
    {
        png_set_strip_16(png);
    }
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png);
    }

    //	int number_of_passes	= png_set_interlace_handling(png);
    png_read_update_info(png, info);

    m_width = png_get_image_width(png, info);
    m_height = png_get_image_height(png, info);
    m_pitch = png_get_rowbytes(png, info);
    m_bpp = png_get_bit_depth(png, info) * png_get_channels(png, info);

    // read file
    if(setjmp(png_jmpbuf(png)) != 0)
    {
        std::cout << "Error during read_image" << std::endl;
        reset();
        return false;
    }

    // create buffer and read data
    png_bytep* row_pointers = new png_bytep[m_height];
    for(unsigned y = 0; y < m_height; y++)
    {
        row_pointers[y] = new png_byte[m_pitch];
    }
    png_read_image(png, row_pointers);

    // create BGRA buffer and decode image data
    m_bitmap.resize(m_pitch * m_height);

    color_type = png_get_color_type(png, info);
    if(color_type == PNG_COLOR_TYPE_RGB)
    {
        m_format = GL_RGB;
        for(unsigned y = 0; y < m_height; y++)
        {
            unsigned dst = y * m_pitch;
            for(unsigned x = 0; x < m_width; x++)
            {
                unsigned dx = x * 3;
                m_bitmap[dst + dx + 0]	= *(row_pointers[y] + dx + 0);
                m_bitmap[dst + dx + 1]	= *(row_pointers[y] + dx + 1);
                m_bitmap[dst + dx + 2]	= *(row_pointers[y] + dx + 2);
            }

            int percent = (int)(100.0f * y / m_height);
            progress(percent);

            delete[] row_pointers[y];
        }
    }
    else if(color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        m_format = GL_RGBA;
        for(unsigned y = 0; y < m_height; y++)
        {
            unsigned dst = y * m_pitch;
            for(unsigned x = 0; x < m_width; x++)
            {
                unsigned dx	= x * 4;
                m_bitmap[dst + dx + 0] = *(row_pointers[y] + dx + 0);
                m_bitmap[dst + dx + 1] = *(row_pointers[y] + dx + 1);
                m_bitmap[dst + dx + 2] = *(row_pointers[y] + dx + 2);
                m_bitmap[dst + dx + 3] = *(row_pointers[y] + dx + 3);
            }

            int percent = (int)(100.0f * y / m_height);
            progress(percent);

            delete[] row_pointers[y];
        }
    }
    else
    {
        for(unsigned y = 0; y < m_height; y++)
        {
            delete[] row_pointers[y];
        }
        std::cout << "Should't be happened" << std::endl;
    }

    delete[] row_pointers;

    png_destroy_read_struct(&png, &info, NULL);

    return true;
}

