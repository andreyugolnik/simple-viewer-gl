/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatico.h"
#include <string.h>
#include <math.h>

using namespace FORMAT_ICO;

CFormatIco::CFormatIco(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

CFormatIco::~CFormatIco()
{
}

bool CFormatIco::Load(const char* filename, unsigned subImage)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    IcoHeader header;
    if(sizeof(header) != file.read(&header, sizeof(header)))
    {
        return false;
    }

    IcoDirentry* images	= new IcoDirentry[header.count];
    if(sizeof(IcoDirentry) * header.count != file.read(images, sizeof(IcoDirentry) * header.count))
    {
        delete[] images;
        return false;
    }

    subImage = std::max<unsigned>(subImage, 0);
    subImage = std::min<unsigned>(subImage, header.count - 1);
    IcoDirentry* image = &images[subImage];
    //	std::cout << std::endl;
    //	std::cout << "--- IcoDirentry ---" << std::endl;
    //	std::cout << "width: " << (int)image->width << "." << std::endl;
    //	std::cout << "height: " << (int)image->height << "." << std::endl;
    //	std::cout << "colors: " << (int)image->colors << "." << std::endl;
    //	std::cout << "planes: " << (int)image->planes << "." << std::endl;
    //	std::cout << "bits: " << (int)image->bits << "." << std::endl;
    //	std::cout << "size: " << (int)image->size << "." << std::endl;
    //	std::cout << "offset: " << (int)image->offset << "." << std::endl;

    bool result = false;

    if(image->colors == 0 && image->width == 0 && image->height == 0)
    {
        result = loadPngFrame(file, image);
    }
    else
    {
        result = loadOrdinaryFrame(file, image);
    }

    delete[] images;

    // store frame number and frames count after reset again
    m_subImage = subImage;
    m_subCount = header.count;

    return result;
}

// load frame in png format
PngRaw CFormatIco::m_pngRaw;
void CFormatIco::readPngData(png_structp /*png*/, png_bytep out, png_size_t count)
{
    // PngRaw& pngRaw	= *(PngRaw*)png->io_ptr;
    if(m_pngRaw.pos + count <= m_pngRaw.size)
    {
        memcpy((uint8_t*)out, &m_pngRaw.data[m_pngRaw.pos], count);
    }
    m_pngRaw.pos += count;
}

bool CFormatIco::loadPngFrame(cFile& file, const IcoDirentry* image)
{
    uint8_t* p = new uint8_t[image->size];

    file.seek(image->offset, SEEK_SET);
    if(image->size != file.read(p, image->size))
    {
        delete[] p;
        return false;
    }

    if(image->size != 8 && png_sig_cmp(p, 0, 8) != 0)
    {
        std::cout << "Frame is not recognized as a PNG format" << std::endl;
        delete[] p;
        return false;
    }

    // initialize stuff
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(png == 0)
    {
        std::cout << "png_create_read_struct failed" << std::endl;
        delete[] p;
        return false;
    }

    m_pngRaw.data = p;
    m_pngRaw.size = image->size;
    m_pngRaw.pos = 8;
    png_set_read_fn(png, &m_pngRaw, readPngData);

    png_infop info = png_create_info_struct(png);
    if(info == 0)
    {
        std::cout << "png_create_info_struct failed" << std::endl;
        delete[] p;
        return false;
    }

    if(setjmp(png_jmpbuf(png)) != 0)
    {
        std::cout << "Error during init_io" << std::endl;
        delete[] p;
        return false;
    }

    png_init_io(png, file.getHandle());
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);

    // get real bits per pixel
    m_bppImage = png_get_bit_depth(png, info) * png_get_channels(png, info);

    //if(info->color_type == PNG_COLOR_TYPE_PALETTE)
    uint8_t color_type = png_get_color_type(png, info);
    if(color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }

#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
    if(info->color_type == PNG_COLOR_TYPE_GRAY && info->bit_depth < 8)
    {
        // depreceted in libPNG-1.4.2
        png_set_gray_1_2_4_to_8(png);
    }
#endif

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
    m_height =png_get_image_height(png, info);
    m_pitch = png_get_rowbytes(png, info);
    m_bpp = png_get_bit_depth(png, info) * png_get_channels(png, info);

    // read file
    if(setjmp(png_jmpbuf(png)) != 0)
    {
        std::cout << "Error during read_image" << std::endl;
        delete[] p;
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
                m_bitmap[dst + dx + 0] = *(row_pointers[y] + dx + 0);
                m_bitmap[dst + dx + 1] = *(row_pointers[y] + dx + 1);
                m_bitmap[dst + dx + 2] = *(row_pointers[y] + dx + 2);
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
                unsigned dx = x * 4;
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

// load frame in ordinary format
bool CFormatIco::loadOrdinaryFrame(cFile& file, const IcoDirentry* image)
{
    file.seek(image->offset, SEEK_SET);
    uint8_t* p = new uint8_t[image->size];
    if(image->size != file.read(p, image->size))
    {
        delete[] p;
        return false;
    }

    IcoBmpInfoHeader* imgHeader = (IcoBmpInfoHeader*)p;
    m_width = imgHeader->width;
    m_height = imgHeader->height / 2;	// xor mask + and mask
    m_pitch = m_width * 4;
    m_bpp = 32;
    m_bppImage = imgHeader->bits;
    m_format = GL_RGBA;

    int pitch = calcIcoPitch();
    if(pitch == -1)
    {
        delete[] p;
        return false;
    }

    m_bitmap.resize(m_pitch * m_height);

    //	std::cout << std::endl;
    //	std::cout << "--- IcoBmpInfoHeader ---" << std::endl;
    //	std::cout << "size: " << (int)imgHeader->size << "." << std::endl;
    //	std::cout << "width: " << (int)imgHeader->width << "." << std::endl;
    //	std::cout << "height: " << (int)imgHeader->height << "." << std::endl;
    //	std::cout << "planes: " << (int)imgHeader->planes << "." << std::endl;
    //	std::cout << "bits: " << (int)imgHeader->bits << "." << std::endl;
    //	std::cout << "imagesize: " << (int)imgHeader->imagesize << "." << std::endl;


    int colors = image->colors == 0 ? (1 << m_bppImage) : image->colors;
    uint32_t* palette = (uint32_t*)&p[imgHeader->size];
    uint8_t* xorMask = &p[imgHeader->size + colors * 4];
    uint8_t* andMask = &p[imgHeader->size + colors * 4 + m_height * pitch];

    switch(m_bppImage)
    {
    case 1:
        for(unsigned y = 0; y < m_height; y++)
        {
            for(unsigned x = 0; x < m_width; x++)
            {
                uint32_t color = palette[getBit(xorMask, y * m_width + x)];

                unsigned idx = (m_height - y - 1) * m_pitch + x * 4;

                m_bitmap[idx + 0] = ((uint8_t*)(&color))[2];
                m_bitmap[idx + 1] = ((uint8_t*)(&color))[1];
                m_bitmap[idx + 2] = ((uint8_t*)(&color))[0];

                if(getBit(andMask, y * m_width + x))
                {
                    m_bitmap[idx + 3] = 0;
                }
                else
                {
                    m_bitmap[idx + 3] = 255;
                }

                int percent	= (int)(100.0f * m_height * m_width / (y * m_width + x));
                progress(percent);
            }
        }
        break;

    case 4:
        for(unsigned y = 0; y < m_height; y++)
        {
            for(unsigned x = 0; x < m_width; x++)
            {
                uint32_t color = palette[getNibble(xorMask, y * m_width + x)];

                unsigned idx = (m_height - y - 1) * m_pitch + x * 4;

                m_bitmap[idx + 0] = ((uint8_t*)(&color))[2];
                m_bitmap[idx + 1] = ((uint8_t*)(&color))[1];
                m_bitmap[idx + 2] = ((uint8_t*)(&color))[0];

                if(getBit(andMask, y * m_width + x))
                {
                    m_bitmap[idx + 3] = 0;
                }
                else
                {
                    m_bitmap[idx + 3] = 255;
                }

                int percent	= (int)(100.0f * m_height * m_width / (y * m_width + x));
                progress(percent);
            }
        }
        break;

    case 8:
        for(unsigned y = 0; y < m_height; y++)
        {
            for(unsigned x = 0; x < m_width; x++)
            {
                uint32_t color = palette[getByte(xorMask, y * m_width + x)];

                unsigned idx = (m_height - y - 1) * m_pitch + x * 4;

                m_bitmap[idx + 0] = ((uint8_t*)(&color))[2];
                m_bitmap[idx + 1] = ((uint8_t*)(&color))[1];
                m_bitmap[idx + 2] = ((uint8_t*)(&color))[0];

                if(getBit(andMask, y * m_width + x))
                {
                    m_bitmap[idx + 3] = 0;
                }
                else
                {
                    m_bitmap[idx + 3] = 255;
                }

                int percent = (int)(100.0f * m_height * m_width / (y * m_width + x));
                progress(percent);
            }
        }
        break;

    default:
        {
            unsigned bpp = m_bppImage / 8;
            for(unsigned y = 0; y < m_height; y++)
            {

                uint8_t* row = xorMask + pitch * y;

                for(unsigned x = 0; x < m_width; x++)
                {
                    unsigned idx = (m_height - y - 1) * m_pitch + x * 4;

                    m_bitmap[idx + 0] = row[2];
                    m_bitmap[idx + 1] = row[1];
                    m_bitmap[idx + 2] = row[0];

                    if(m_bppImage < 32)
                    {
                        if(getBit(andMask, y * m_width + x))
                        {
                            m_bitmap[idx + 3] = 0;
                        }
                        else
                        {
                            m_bitmap[idx + 3] = 255;
                        }
                    }
                    else
                    {
                        m_bitmap[idx + 3] = row[3];
                    }

                    row += bpp;

                    int percent = (int)(100.0f * m_height * m_width / (y * m_width + x));
                    progress(percent);
                }
            }
        }
        break;
    }

    delete[] p;

    return true;
}

int CFormatIco::calcIcoPitch()
{
    switch(m_bppImage)
    {
    case 1:
        if((m_width % 32) == 0)
            return m_width / 8;
        return 4 * (m_width / 32 + 1);

    case 4:
        if((m_width % 8) == 0)
            return m_width / 2;
        return 4 * (m_width / 8 + 1);

    case 8:
        if((m_width % 4) == 0)
            return m_width;
        return 4 * (m_width / 4 + 1);

    case 24:
        if(((m_width * 3) % 4) == 0)
            return m_width * 3;
        return 4 * (m_width * 3 / 4 + 1);

    case 32:
        return m_width * 4;

    default:
        std::cout << "Invalid bits count: " << m_bppImage << std::endl;
        return -1;	//m_width * (m_bppImage / 8);
    }
}

int CFormatIco::getBit(const uint8_t* data, int bit)
{
    // width per line in multiples of 32 bits
    int width32 = (m_width % 32 == 0 ? m_width / 32 : m_width / 32 + 1);
    int line = bit / m_width;
    int offset = bit % m_width;

    int result = (data[line * width32 * 4 + offset / 8] & (1 << (7 - (offset % 8))));

    return (result ? 1 : 0);
}

int CFormatIco::getNibble(const uint8_t* data, int nibble)
{
    // width per line in multiples of 32 bits
    int width32 = (m_width % 8 == 0 ? m_width / 8 : m_width / 8 + 1);
    int line = nibble / m_width;
    int offset = nibble % m_width;

    int result = (data[line * width32 * 4 + offset / 2] & (0x0F << (4 * (1 - offset % 2))));

    if(offset % 2 == 0)
    {
        result = result >> 4;
    }

    return result;
}

int CFormatIco::getByte(const uint8_t* data, int byte)
{
    // width per line in multiples of 32 bits
    int width32 = (m_width % 4 == 0 ? m_width / 4 : m_width / 4 + 1);
    int line = byte / m_width;
    int offset = byte % m_width;

    return data[line * width32 * 4 + offset];
}

