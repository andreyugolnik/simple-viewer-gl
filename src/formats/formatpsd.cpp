/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatpsd.h"
#include "helpers.h"

#include <string.h>
#include <iostream>

// http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/
#pragma pack(push, 1)
struct PSD_HEADER
{
    uint8_t signature[4];   // file ID, always "8BPS"
    uint16_t version;       // version number, always 1
    uint8_t resetved[6];
    uint16_t channels;      // number of color channels (1-56)
    uint32_t rows;          // height of image in pixels (1-30000)
    uint32_t columns;       // width of image in pixels (1-30000)
    uint16_t depth;         // number of bits per channel (1, 8, 16, 32)
    uint16_t color_mode;    // color mode as defined below
};
#pragma pack(pop)

enum COLOR_MODE
{
    PSD_MODE_MONO         = 0,
    PSD_MODE_GRAYSCALE    = 1,
    PSD_MODE_INDEXED      = 2,
    PSD_MODE_RGB          = 3,
    PSD_MODE_CMYK         = 4,
    // PSD_MODE_UNUSED    = 5,
    // PSD_MODE_UNUSED    = 6,
    PSD_MODE_MULTICHANNEL = 7,
    PSD_MODE_DUOTONE      = 8,
    PSD_MODE_LAB          = 9
};

static const char* modeToString(unsigned mode)
{
    static const char* modes[] =
    {
        "MONO",
        "GRAYSCALE",
        "INDEXED",
        "RGB",
        "CMYK",
        "unknown",
        "unknown",
        "MULTICHANNEL",
        "DUOTONE",
        "LAB",
        "unknown"
    };
    static const size_t size = sizeof(modes)/sizeof(modes[0]);

    if(mode < size)
    {
        return modes[mode];
    }
    return modes[size - 1];
}

enum COMPRESSION_METHOD
{
    COMP_RAW = 0,         // Raw image data
    COMP_RLE = 1,         // RLE compressed the image data starts with the byte counts
                          // for all the scan lines (rows * channels), with each count
                          // stored as a two-byte value. The RLE compressed data follows,
                          // with each scan line compressed separately. The RLE compression
                          // is the same compression algorithm used by the Macintosh ROM
                          // routine PackBits, and the TIFF standard.
    COMP_ZIP = 2,         // ZIP without prediction
    COMP_ZIP_PREDICT = 3  // ZIP with prediction
};

static bool skipNextBlock(cFile& file)
{
    uint32_t size;
    if(sizeof(uint32_t) != file.read(&size, sizeof(uint32_t)))
    {
        return false;
    }
    size = read_uint32((uint8_t*)&size);
    //printf("%u bytes skipped\n", size);
    file.seek(size, SEEK_CUR);

    return true;
}

static void decodeRle(uint8_t* dst, const uint8_t* src, uint32_t lineLength)
{
    uint16_t bytes_read = 0;
    while(bytes_read < lineLength)
    {
        const signed char byte = src[bytes_read];
        bytes_read++;

        if(byte == -128)
        {
            continue;
        }
        else if(byte > -1)
        {
            const int count = byte + 1;

            // copy next count bytes
            for(int i = 0; i < count; i++)
            {
                *dst = src[bytes_read];
                dst++;
                bytes_read++;
            }
        }
        else
        {
            const int count = -byte + 1;

            // copy next byte count times
            const uint8_t next_byte = src[bytes_read];
            bytes_read++;
            for(int i = 0; i < count; i++)
            {
                *dst = next_byte;
                dst++;
            }
        }
    }
}

CFormatPsd::CFormatPsd(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

CFormatPsd::~CFormatPsd()
{
}

template<typename C>
void fromRgba(unsigned char* bitmap, const C* r, const C* g, const C* b, const C* a, unsigned w, unsigned h)
{
    const unsigned shift = (unsigned)sizeof(C) >> 1;
    for(unsigned y = 0; y < h; y++)
    {
        unsigned idx = w * y;
        for(unsigned x = 0; x < w; x++)
        {
            bitmap[0] = r[idx] >> shift;
            bitmap[1] = g[idx] >> shift;
            bitmap[2] = b[idx] >> shift;
            bitmap[3] = a[idx] >> shift;
            bitmap += 4;
            idx++;
        }
    }
}

template<>
void fromRgba(unsigned char* bitmap, const uint32_t* r, const uint32_t* g, const uint32_t* b, const uint32_t* a, unsigned w, unsigned h)
{
    for(unsigned y = 0; y < h; y++)
    {
        unsigned idx = w * y;
        for(unsigned x = 0; x < w; x++)
        {
            const uint8_t* ur = (const uint8_t*)&r[idx];
            const uint8_t* ug = (const uint8_t*)&g[idx];
            const uint8_t* ub = (const uint8_t*)&b[idx];
            const uint8_t* ua = (const uint8_t*)&a[idx];
            bitmap[0] = ur[1];
            bitmap[1] = ug[1];
            bitmap[2] = ub[1];
            bitmap[3] = ua[1];
            bitmap += 4;
            idx++;
        }
    }
}

bool CFormatPsd::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    PSD_HEADER header;
    if(sizeof(PSD_HEADER) != file.read(&header, sizeof(PSD_HEADER)))
    {
        printf("Can't read PSD header\n");
        return false;
    }

    const uint16_t version = read_uint16((uint8_t*)&header.version);
    if(version != 1
            || header.signature[0] != '8'
            || header.signature[1] != 'B'
            || header.signature[2] != 'P'
            || header.signature[3] != 'S')
    {
        printf("Not valid PSD file\n");
        return false;
    }

    const unsigned color_mode = read_uint16((uint8_t*)&header.color_mode);
    if(color_mode != PSD_MODE_RGB && color_mode != PSD_MODE_CMYK)// && color_mode != PSD_MODE_GRAYSCALE)
    {
        printf("Unsupported color mode: %s\n",  modeToString(color_mode));
        return false;
    }

    const unsigned depth = read_uint16((uint8_t*)&header.depth);
    if(depth != 8 && depth != 16)// && depth != 32)
    {
        printf("Unsupported depth: %u\n", depth);
        return false;
    }
    const unsigned bytes_per_component = depth / 8;

    const unsigned channels = read_uint16((uint8_t*)&header.channels);

    // skip Color Mode Data Block
    if(false == skipNextBlock(file))
    {
        printf("Can't read Color Mode Data Block\n");
        return false;
    }

    // skip Image Resources Block
    if(false == skipNextBlock(file))
    {
        printf("Can't read Image Resources Block\n");
        return false;
    }

    // skip Layer and Mask Information Block
    if(false == skipNextBlock(file))
    {
        printf("Can't read Layer and Mask Information Block\n");
        return false;
    }

    // Image Data Block
    uint16_t compression;
    if(sizeof(uint16_t) != file.read(&compression, sizeof(uint16_t)))
    {
        printf("Can't read compression info\n");
        return false;
    }
    compression = read_uint16((uint8_t*)&compression);
    if(compression != COMP_RAW && compression != COMP_RLE)
    {
        printf("Unsupported compression: %u\n", compression);
        return false;
    }

    m_width = read_uint32((uint8_t*)&header.columns);
    m_height = read_uint32((uint8_t*)&header.rows);

    // this will be needed for RLE decompression
    std::vector<uint16_t> linesLengths;
    if(compression == COMP_RLE)
    {
        linesLengths.resize(channels * m_height);
        for(unsigned ch = 0; ch < channels; ch++)
        {
            const unsigned pos = m_height * ch;

            if(m_height * sizeof(uint16_t) != file.read(&linesLengths[pos], m_height * sizeof(uint16_t)))
            {
                printf("Can't read length of lines\n");
                return false;
            }
        }

        // convert from different endianness
        for(unsigned i = 0; i < m_height * channels; i++)
        {
            linesLengths[i] = read_uint16((uint8_t*)&linesLengths[i]);
        }
    }

    // only first 3 or 4 channels used
    m_bpp = 8 * std::min<unsigned>(channels, 4);
    m_bppImage = depth * channels;

    // we need buffer that can contain one channel data of one
    // row in RLE compressed format. 2*width should be enough
    const unsigned max_line_length = m_width * 2 * bytes_per_component;
    std::vector<uint8_t> buffer(max_line_length);

    // create separate buffers for each channel (up to 56 buffers by spec)
    std::vector<uint8_t*> chBufs(channels);
    for(unsigned ch = 0; ch < channels; ch++)
    {
        chBufs[ch] = new uint8_t[m_width * m_height * bytes_per_component];
    }

    // read all channels rgba and extra if available;
    for(unsigned ch = 0; ch < channels; ch++)
    {
        unsigned pos = 0;
        for(unsigned row = 0; row < m_height; row++)
        {
            if(compression == COMP_RLE)
            {
                unsigned lineLength = linesLengths[ch * m_height + row] * bytes_per_component;
                if(max_line_length < lineLength)
                {
                    printf("Wrong line length: %u\n", lineLength);
                    lineLength = max_line_length;
                }

                const size_t readed = file.read(&buffer[0], lineLength);
                if(lineLength != readed)
                {
                    printf("Error reading Image Data Block\n");
                }

                decodeRle(chBufs[ch] + pos, &buffer[0], lineLength);
            }
            else
            {
                unsigned lineLength = m_width * bytes_per_component;

                const size_t readed = file.read(chBufs[ch] + pos, lineLength);
                if(lineLength != readed)
                {
                    printf("Error reading Image Data Block\n");
                }
            }

            int percent = (int)(100.0f * (ch * m_height + row) / (channels * m_height));
            progress(percent);

            pos += m_width * bytes_per_component;
        }
    }

    // convert or copy channel buffers to RGB / RGBA
    m_pitch = m_width * std::min<unsigned>(channels, 4);
    m_bitmap.resize(m_pitch * m_height);
    unsigned char* bitmap = &m_bitmap[0];

    if(color_mode == PSD_MODE_RGB)
    {
        if(channels == 3)
        {
            m_format = GL_RGB;
            for(unsigned y = 0; y < m_height; y++)
            {
                for(unsigned x = 0; x < m_width; x++)
                {
                    const unsigned idx = (m_width * y + x) * bytes_per_component;
                    bitmap[0] = *(chBufs[0] + idx);
                    bitmap[1] = *(chBufs[1] + idx);
                    bitmap[2] = *(chBufs[2] + idx);
                    bitmap += 3;
                }
            }
        }
        else
        {
            m_format = GL_RGBA;
            switch(depth)
            {
            case 8:
                {
                    uint8_t* r = chBufs[0];
                    uint8_t* g = chBufs[1];
                    uint8_t* b = chBufs[2];
                    uint8_t* a = chBufs[3];
                    fromRgba(bitmap, r, g, b, a, m_width, m_height);
                }
                break;
            case 16:
                {
                    uint16_t* r = (uint16_t*)chBufs[0];
                    uint16_t* g = (uint16_t*)chBufs[1];
                    uint16_t* b = (uint16_t*)chBufs[2];
                    uint16_t* a = (uint16_t*)chBufs[3];
                    fromRgba(bitmap, r, g, b, a, m_width, m_height);
                }
                break;
            case 32:
                {
                    uint32_t* r = (uint32_t*)chBufs[0];
                    uint32_t* g = (uint32_t*)chBufs[1];
                    uint32_t* b = (uint32_t*)chBufs[2];
                    uint32_t* a = (uint32_t*)chBufs[3];
                    fromRgba(bitmap, r, g, b, a, m_width, m_height);
                }
                break;
            }
        }
    }
    else if(color_mode == PSD_MODE_CMYK)
    {
        if(channels == 4)
        {
            m_format = GL_RGB;
            for(unsigned y = 0; y < m_height; y++)
            {
                for(unsigned x = 0; x < m_width; x++)
                {
                    const unsigned idx = (m_width * y + x) * bytes_per_component;
                    const double C = 1.0 - *(chBufs[0] + idx) / 255.0; // C
                    const double M = 1.0 - *(chBufs[1] + idx) / 255.0; // M
                    const double Y = 1.0 - *(chBufs[2] + idx) / 255.0; // Y
                    const double K = 1.0 - *(chBufs[3] + idx) / 255.0; // K
                    const double Kinv = 1.0 - K;

                    bitmap[0] = (unsigned char)((1.0 - (C * Kinv + K)) * 255.0);
                    bitmap[1] = (unsigned char)((1.0 - (M * Kinv + K)) * 255.0);
                    bitmap[2] = (unsigned char)((1.0 - (Y * Kinv + K)) * 255.0);
                    bitmap += 3;
                }
            }
        }
        else if(channels == 5)
        {
            m_format = GL_RGBA;
            for(unsigned y = 0; y < m_height; y++)
            {
                for(unsigned x = 0; x < m_width; x++)
                {
                    const unsigned idx = (m_width * y + x) * bytes_per_component;
                    const double C = 1.0 - *(chBufs[0] + idx) / 255.0; // C
                    const double M = 1.0 - *(chBufs[1] + idx) / 255.0; // M
                    const double Y = 1.0 - *(chBufs[2] + idx) / 255.0; // Y
                    const double K = 1.0 - *(chBufs[3] + idx) / 255.0; // K
                    const double Kinv = 1.0 - K;

                    bitmap[0] = (unsigned char)(((1.0 - C) * Kinv) * 255.0);
                    bitmap[1] = (unsigned char)(((1.0 - M) * Kinv) * 255.0);
                    bitmap[2] = (unsigned char)(((1.0 - Y) * Kinv) * 255.0);
                    bitmap[3] = *(chBufs[4] + idx); // Alpha
                    bitmap += 4;
                }
            }
        }
    }
    else if(color_mode == PSD_MODE_GRAYSCALE)
    {
        printf("-- compressio: %u, ch: %u, depth: %u, bytes: %u\n", compression, channels, depth, bytes_per_component);

        if(channels == 2)
        {
            m_pitch = m_width * 4;
            m_bitmap.resize(m_pitch * m_height);
            unsigned char* bitmap = &m_bitmap[0];

            m_format = GL_RGBA;
            switch(depth)
            {
            case 8:
                {
                    uint8_t* c = chBufs[0];
                    uint8_t* a = chBufs[1];
                    fromRgba(bitmap, c, c, c, a, m_width, m_height);
                }
                break;
            case 16:
                {
                    uint16_t* c = (uint16_t*)chBufs[0];
                    uint16_t* a = (uint16_t*)chBufs[1];
                    fromRgba(bitmap, c, c, c, a, m_width, m_height);
                }
                break;
            case 32:
                {
                    uint32_t* c = (uint32_t*)chBufs[0];
                    uint32_t* a = (uint32_t*)chBufs[1];
                    fromRgba(bitmap, c, c, c, a, m_width, m_height);
                }
                break;
            }
        }
        else if(channels == 1)
        {
            m_pitch = m_width * 3;
            m_bitmap.resize(m_pitch * m_height);
            unsigned char* bitmap = &m_bitmap[0];

            m_format = GL_RGB;
            for(unsigned y = 0; y < m_height; y++)
            {
                for(unsigned x = 0; x < m_width; x++)
                {
                    const unsigned idx = (m_width * y + x) * bytes_per_component;
                    bitmap[0] = *(chBufs[0] + idx);
                    bitmap[1] = *(chBufs[0] + idx);
                    bitmap[2] = *(chBufs[0] + idx);
                    bitmap += 3;
                }
            }
        }
    }

    for(unsigned ch = 0, size = chBufs.size(); ch < size; ch++)
    {
        delete[] chBufs[ch];
    }

    return true;
}

