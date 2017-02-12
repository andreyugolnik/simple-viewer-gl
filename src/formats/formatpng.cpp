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
#include "../common/helpers.h"

#include <cstring>
#include <png.h>

namespace
{

    void* locateICCProfile(const png_structp png, const png_infop info, unsigned& iccProfileSize)
    {
        png_charp name;
        int comp_type;
#if ((PNG_LIBPNG_VER_MAJOR << 8) | PNG_LIBPNG_VER_MINOR << 0) < \
        ((1 << 8) | (5 << 0))
        png_charp icc;
#else  // >= libpng 1.5.0
        png_bytep icc;
#endif
        png_uint_32 size;
        if (png_get_iCCP(png, info, &name, &comp_type, &icc, &size) == PNG_INFO_iCCP)
        {
            // ::printf("-- name: %s\n", name);
            // ::printf("-- comp_type: %d\n", comp_type);
            // ::printf("-- size: %u\n", size);

            iccProfileSize = size;
            return icc;
        }

        return nullptr;
    }

    struct PngHeader
    {
        png_byte id[8]; // 8 is the maximum size that can be checked
    };

    bool isValidFormat(PngHeader& header, unsigned fileSize)
    {
        return fileSize >= 8 && png_sig_cmp(header.id, 0, 8) == 0;
    }

}

cFormatPng::cFormatPng(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatPng::~cFormatPng()
{
}

bool cFormatPng::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, sizeof(PngHeader)))
    {
        return false;
    }

    PngHeader header;
    ::memcpy(&header, buffer.data(), sizeof(header));
    return isValidFormat(header, file.getSize());
}

bool cFormatPng::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    PngHeader header;
    if (file.read(&header, sizeof(header)) != sizeof(header)
        && isValidFormat(header, file.getSize()) == false)
    {
        ::printf("(EE) Is not recognized as a PNG file.\n");
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
    desc.bpp = png_get_bit_depth(png, info) * png_get_channels(png, info);
    desc.pitch = helpers::calculatePitch(desc.width, desc.bpp / 8);//png_get_rowbytes(png, info);

    // read file
    if (setjmp(png_jmpbuf(png)) != 0)
    {
        ::printf("(EE) Error during read_image.\n");
        return false;
    }

    // create buffer and read data
    auto row_pointers = new png_bytep[desc.height];
    for (unsigned y = 0; y < desc.height; y++)
    {
        row_pointers[y] = new png_byte[desc.pitch];
    }
    png_read_image(png, row_pointers);

    // create RGBA buffer and decode image data
    desc.bitmap.resize(desc.pitch * desc.height);
    auto out = desc.bitmap.data();

    std::vector<unsigned char> buffer(desc.pitch);
    auto input = buffer.data();

    color_type = png_get_color_type(png, info);

    unsigned iccProfileSize = 0;
    auto iccProfile = locateICCProfile(png, info, iccProfileSize);
    m_cms.createTransform(iccProfile, iccProfileSize, cCMS::Pixel::Rgb);

    m_formatName = m_cms.hasTransform() ? "png/icc" : "png";

    if (color_type == PNG_COLOR_TYPE_RGB)
    {
        desc.format = GL_RGB;
        for (unsigned y = 0; y < desc.height; y++)
        {
            if (m_cms.hasTransform())
            {
                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dx = x * 3;
                    input[dx + 0] = *(row_pointers[y] + dx + 0);
                    input[dx + 1] = *(row_pointers[y] + dx + 1);
                    input[dx + 2] = *(row_pointers[y] + dx + 2);
                }

                m_cms.doTransform(input, out, desc.width);
            }
            else
            {
                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dx = x * 3;
                    out[dx + 0] = *(row_pointers[y] + dx + 0);
                    out[dx + 1] = *(row_pointers[y] + dx + 1);
                    out[dx + 2] = *(row_pointers[y] + dx + 2);
                }
            }
            out += desc.pitch;

            updateProgress((float)y / desc.height);

            delete[] row_pointers[y];
        }
    }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        desc.format = GL_RGBA;
        for (unsigned y = 0; y < desc.height; y++)
        {
            if (m_cms.hasTransform())
            {
                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dx = x * 4;
                    input[x * 3 + 0] = *(row_pointers[y] + dx + 0);
                    input[x * 3 + 1] = *(row_pointers[y] + dx + 1);
                    input[x * 3 + 2] = *(row_pointers[y] + dx + 2);
                }

                m_cms.doTransform(input, input, desc.width);

                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dx = x * 4;
                    out[dx + 0] = input[x * 3 + 0];
                    out[dx + 1] = input[x * 3 + 1];
                    out[dx + 2] = input[x * 3 + 2];
                    out[dx + 3] = *(row_pointers[y] + dx + 3);
                }
            }
            else
            {
                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dx = x * 4;
                    out[dx + 0] = *(row_pointers[y] + dx + 0);
                    out[dx + 1] = *(row_pointers[y] + dx + 1);
                    out[dx + 2] = *(row_pointers[y] + dx + 2);
                    out[dx + 3] = *(row_pointers[y] + dx + 3);
                }
            }
            out += desc.pitch;

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

    m_cms.destroyTransform();

    return true;
}
