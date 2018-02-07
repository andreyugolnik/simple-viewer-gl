/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "PngReader.h"
#include "cms/cms.h"
#include "common/bitmap_description.h"
#include "common/helpers.h"

#include <cstring>
#include <png.h>

namespace
{
    class cPngMemoryReader
    {
    public:
        cPngMemoryReader(const uint8_t* data, uint32_t size)
            : m_data(data)
            , m_remain(size)
            , m_offset(0)
        {
        }

        uint32_t read(uint8_t* out, uint32_t size)
        {
            size = size <= m_remain ? size : m_remain;

            ::memcpy(out, &m_data[m_offset], size);
            m_offset += size;
            m_remain -= size;

            return size;
        }

        static void memoryReader(png_structp png, png_bytep outBytes, png_size_t byteCountToRead)
        {
            auto reader = static_cast<cPngMemoryReader*>(png_get_io_ptr(png));
            if (reader != nullptr)
            {
                reader->read(outBytes, byteCountToRead);
            }
        }

    private:
        const uint8_t* m_data;
        uint32_t m_remain;
        uint32_t m_offset;
    };

    void* locateICCProfile(const png_structp png, const png_infop info, uint32_t& iccProfileSize)
    {
        png_charp name;
        int comp_type;
#if ((PNG_LIBPNG_VER_MAJOR << 8) | PNG_LIBPNG_VER_MINOR << 0) < ((1 << 8) | (5 << 0))
        png_charp icc;
#else // >= libpng 1.5.0
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

} // namespace

cPngReader::cPngReader(cCMS& cms)
    : m_cms(cms)
{
}

cPngReader::~cPngReader()
{
}

bool cPngReader::loadPng(sBitmapDescription& desc, const uint8_t* data, uint32_t size) const
{
    if (size < 8 || png_sig_cmp(data, 0, 8) != 0)
    {
        ::printf("(EE) Frame is not recognized as a PNG format.\n");
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
        png_destroy_read_struct(&png, nullptr, nullptr);
        return false;
    }

    cPngMemoryReader pngReader{ data, size };
    png_set_read_fn(png, &pngReader, cPngMemoryReader::memoryReader);

    png_read_info(png, info);

    auto colorType = png_get_color_type(png, info);
    if (colorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }

#if defined(PNG_1_0_X) || defined(PNG_1_2_X)
    if (colorType == PNG_COLOR_TYPE_GRAY && info->bit_depth < 8)
    {
        // depreceted in libPNG-1.4.2
        png_set_gray_1_2_4_to_8(png);
    }
#endif

    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
    }
    if (png_get_bit_depth(png, info) == 16)
    {
        png_set_strip_16(png);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png);
    }

    // int number_of_passes = png_set_interlace_handling(png);
    png_read_update_info(png, info);

    desc.width = png_get_image_width(png, info);
    desc.height = png_get_image_height(png, info);
    desc.bpp = png_get_bit_depth(png, info) * png_get_channels(png, info);
    desc.pitch = helpers::calculatePitch(desc.width, desc.bpp); //png_get_rowbytes(png, info);
    if (desc.pitch < png_get_rowbytes(png, info))
    {
        ::printf("(EE) Invalid pitch: %u instead %u.\n", desc.pitch, (uint32_t)png_get_rowbytes(png, info));
    }

    colorType = png_get_color_type(png, info);

    desc.bitmap.resize(desc.pitch * desc.height);
    auto out = desc.bitmap.data();
    std::vector<png_bytep> rows(desc.height);
    for (uint32_t y = 0; y < desc.height; y++)
    {
        rows[y] = out + desc.pitch * y;
    }
    png_read_image(png, rows.data());

    uint32_t iccProfileSize = 0;
    auto iccProfile = locateICCProfile(png, info, iccProfileSize);
    m_cms.createTransform(iccProfile, iccProfileSize, cCMS::Pixel::Rgb);

    if (colorType == PNG_COLOR_TYPE_RGB)
    {
        desc.format = GL_RGB;

        if (m_cms.hasTransform())
        {
            for (uint32_t y = 0; y < desc.height; y++)
            {
                auto input = rows[y];
                m_cms.doTransform(input, input, desc.width);

                updateProgress((float)y / desc.height);
            }
        }
    }
    else if (colorType == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        desc.format = GL_RGBA;

        if (m_cms.hasTransform())
        {
            std::vector<uint8_t> buffer(desc.width * 3);
            auto input = buffer.data();

            for (uint32_t y = 0; y < desc.height; y++)
            {
                auto bmp = rows[y];

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t dst = x * 3;
                    const uint32_t src = x * 4;
                    input[dst + 0] = bmp[src + 0];
                    input[dst + 1] = bmp[src + 1];
                    input[dst + 2] = bmp[src + 2];
                }

                m_cms.doTransform(input, input, desc.width);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t dst = x * 4;
                    const uint32_t src = x * 3;
                    bmp[dst + 0] = input[src + 0];
                    bmp[dst + 1] = input[src + 1];
                    bmp[dst + 2] = input[src + 2];
                }

                updateProgress((float)y / desc.height);
            }
        }
    }
    else
    {
        ::printf("(EE) Should't be happened.\n");
    }

    png_destroy_read_struct(&png, &info, nullptr);

    m_cms.destroyTransform();

    return true;
}

bool cPngReader::isValid(const uint8_t* data, uint32_t size) const
{
    return size >= 8 && png_sig_cmp(data, 0, 8) == 0;
}

bool cPngReader::loadPng(sBitmapDescription& desc, cFile& file) const
{
#if 0
    desc.size = file.getSize();

    PngHeader header;
    if (file.read(&header, sizeof(header)) != sizeof(header)
        && isValid(header, file.getSize()) == false)
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
    desc.pitch = helpers::calculatePitch(desc.width, desc.bpp); //png_get_rowbytes(png, info);
    if (desc.pitch < png_get_rowbytes(png, info))
    {
        ::printf("(EE) Invalid pitch: %u instead %u.\n", desc.pitch, (uint32_t)png_get_rowbytes(png, info));
    }

    // read file
    if (setjmp(png_jmpbuf(png)) != 0)
    {
        ::printf("(EE) Error during read_image.\n");
        return false;
    }

    // create buffer and read data
    // auto row_pointers = new png_bytep[desc.height];
    // for (unsigned y = 0; y < desc.height; y++)
    // {
    // row_pointers[y] = new png_byte[desc.pitch];
    // }
    // png_read_image(png, row_pointers);

    // create RGBA buffer and decode image data
    desc.bitmap.resize(desc.pitch * desc.height);
    auto out = desc.bitmap.data();
    std::vector<png_bytep> row_pointers(desc.height);
    for (uint32_t y = 0; y < desc.height; y++)
    {
        row_pointers[y] = out + desc.pitch * y;
    }
    png_read_image(png, row_pointers.data());

    color_type = png_get_color_type(png, info);

    unsigned iccProfileSize = 0;
    auto iccProfile = locateICCProfile(png, info, iccProfileSize);
    m_cms.createTransform(iccProfile, iccProfileSize, cCMS::Pixel::Rgb);

    m_formatName = m_cms.hasTransform() ? "png/icc" : "png";

    if (color_type == PNG_COLOR_TYPE_RGB)
    {
        desc.format = GL_RGB;

        if (m_cms.hasTransform())
        {
            for (unsigned y = 0; y < desc.height; y++)
            {
                auto input = row_pointers[y];
                m_cms.doTransform(input, input, desc.width);

                updateProgress((float)y / desc.height);
            }
        }
    }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        desc.format = GL_RGBA;

        if (m_cms.hasTransform())
        {
            std::vector<uint8_t> buffer(desc.width * 3);
            auto input = buffer.data();

            for (unsigned y = 0; y < desc.height; y++)
            {
                auto bmp = row_pointers[y];

                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dst = x * 3;
                    const unsigned src = x * 4;
                    input[dst + 0] = bmp[src + 0];
                    input[dst + 1] = bmp[src + 1];
                    input[dst + 2] = bmp[src + 2];
                }

                m_cms.doTransform(input, input, desc.width);

                for (unsigned x = 0; x < desc.width; x++)
                {
                    const unsigned dst = x * 4;
                    const unsigned src = x * 3;
                    bmp[dst + 0] = input[src + 0];
                    bmp[dst + 1] = input[src + 1];
                    bmp[dst + 2] = input[src + 2];
                }

                updateProgress((float)y / desc.height);
            }
        }
    }
    else
    {
        ::printf("(EE) Should't be happened.\n");
    }

    png_destroy_read_struct(&png, &info, nullptr);

    m_cms.destroyTransform();

#endif
    return true;
}
