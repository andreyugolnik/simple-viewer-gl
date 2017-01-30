/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatpng.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <png.h>

cFormatPng::cFormatPng(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatPng::~cFormatPng()
{
}

bool cFormatPng::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    png_byte header[8]; // 8 is the maximum size that can be checked
    auto size = file.read(header, 8);
    if (size != 8 || png_sig_cmp(header, 0, 8) != 0)
    {
        ::printf("(EE) File '%s' is not recognized as a PNG file.\n", filename);
        return false;
    }

    // initialize stuff
    auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr)
    {
        ::printf("(EE) png_create_read_struct failed.\n");
        return false;
    }

    auto info = png_create_info_struct(png);
    if (info == nullptr)
    {
        ::printf("(EE) png_create_info_struct failed.\n");
        return false;
    }

    if (setjmp(png_jmpbuf(png)) != 0)
    {
        ::printf("(EE) Error during init_io.\n");
        return false;
    }

    png_init_io(png, (FILE*)file.getHandle());
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);

    // get real bits per pixel
    desc.bppImage = png_get_bit_depth(png, info) * png_get_channels(png, info);

    uint8_t color_type = png_get_color_type(png, info);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
    }
    if (png_get_bit_depth(png, info) == 16)
    {
        png_set_strip_16(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png);
    }

    //  int number_of_passes    = png_set_interlace_handling(png);
    png_read_update_info(png, info);

    desc.width = png_get_image_width(png, info);
    desc.height = png_get_image_height(png, info);
    desc.pitch = png_get_rowbytes(png, info);
    desc.bpp = png_get_bit_depth(png, info) * png_get_channels(png, info);

    // read file
    if (setjmp(png_jmpbuf(png)) != 0)
    {
        ::printf("(EE) Error during read_image.\n");
        return false;
    }

    // create buffer and read data
    png_bytep* row_pointers = new png_bytep[desc.height];
    for (unsigned y = 0; y < desc.height; y++)
    {
        row_pointers[y] = new png_byte[desc.pitch];
    }
    png_read_image(png, row_pointers);

    // create BGRA buffer and decode image data
    desc.bitmap.resize(desc.pitch * desc.height);

    color_type = png_get_color_type(png, info);
    if (color_type == PNG_COLOR_TYPE_RGB)
    {
        desc.format = GL_RGB;
        for (unsigned y = 0; y < desc.height; y++)
        {
            unsigned dst = y * desc.pitch;
            for (unsigned x = 0; x < desc.width; x++)
            {
                unsigned dx = x * 3;
                desc.bitmap[dst + dx + 0] = *(row_pointers[y] + dx + 0);
                desc.bitmap[dst + dx + 1] = *(row_pointers[y] + dx + 1);
                desc.bitmap[dst + dx + 2] = *(row_pointers[y] + dx + 2);
            }

            updateProgress((float)y / desc.height);

            delete[] row_pointers[y];
        }
    }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        desc.format = GL_RGBA;
        for (unsigned y = 0; y < desc.height; y++)
        {
            unsigned dst = y * desc.pitch;
            for (unsigned x = 0; x < desc.width; x++)
            {
                unsigned dx = x * 4;
                desc.bitmap[dst + dx + 0] = *(row_pointers[y] + dx + 0);
                desc.bitmap[dst + dx + 1] = *(row_pointers[y] + dx + 1);
                desc.bitmap[dst + dx + 2] = *(row_pointers[y] + dx + 2);
                desc.bitmap[dst + dx + 3] = *(row_pointers[y] + dx + 3);
            }

            updateProgress((float)y / desc.height);

            delete[] row_pointers[y];
        }
    }
    else
    {
        for (unsigned y = 0; y < desc.height; y++)
        {
            delete[] row_pointers[y];
        }
        ::printf("(EE) Should't be happened.\n");
    }

    delete[] row_pointers;

    png_destroy_read_struct(&png, &info, nullptr);

    m_formatName = "png";

    return true;
}
