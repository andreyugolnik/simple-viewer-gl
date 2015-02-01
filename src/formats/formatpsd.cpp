/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatpsd.h"
#include <string.h>
#include <iostream>

using namespace FORMAT_PSD;

CFormatPsd::CFormatPsd(const char* lib, const char* name)
    : CFormat(lib, name)
    , m_buffer(0)
    , m_linesLengths(0)
{
    memset(m_chBufs, 0, sizeof(m_chBufs));
}

CFormatPsd::~CFormatPsd()
{
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
        std::cout << "Can't read PSD header" << std::endl;
        return false;
    }

    if(header.signature[0] != '8' || header.signature[1] != 'B' || header.signature[2] != 'P' || header.signature[3] != 'S')
    {
        std::cout << "Not valid PSD file" << std::endl;
        return false;
    }

    const unsigned color_mode = read_uint16((uint8_t*)&header.color_mode);
    if(color_mode != PSD_MODE_RGB && color_mode != PSD_MODE_CMYK)
    {
        std::cout << "Unsupported color mode: " << color_mode << std::endl;
        return false;
    }

    const unsigned depth = read_uint16((uint8_t*)&header.depth);
    if(depth != 8)
    {
        std::cout << "Unsupported depth: " << depth << std::endl;
        return false;
    }

    const unsigned channels = read_uint16((uint8_t*)&header.channels);
    if(channels != 3 && channels != 4)
    {
        std::cout << "Unsupported cannels count: " << channels << std::endl;
        //return false;
    }

    // skip Color Mode Data Block
    if(false == skipNextBlock(file))
    {
        std::cout << "Can't read Color Mode Data Block" << std::endl;
        return false;
    }

    // skip Image Resources Block
    if(false == skipNextBlock(file))
    {
        std::cout << "Can't read Image Resources Block" << std::endl;
        return false;
    }

    // Layer and Mask Information Block
    if(false == skipNextBlock(file))
    {
        std::cout << "Can't read Layer and Mask Information Block" << std::endl;
        return false;
    }

    // Image Data Block
    uint16_t compression;
    if(sizeof(uint16_t) != file.read(&compression, sizeof(uint16_t)))
    {
        std::cout << "Can't read compression info" << std::endl;
        return false;
    }
    compression = read_uint16((uint8_t*)&compression);
    //printf("compression: %u\n", compression);

    m_width = read_uint32((uint8_t*)&header.columns);
    m_height = read_uint32((uint8_t*)&header.rows);

    // this will be needed for RLE decompression
    if(compression == 1)
    {
        m_linesLengths = new uint16_t[channels * m_height];
        for(unsigned i = 0; i < channels; i++)
        {
            const unsigned pos = m_height * i;

            if(m_height * sizeof(uint16_t) != file.read(&m_linesLengths[pos], m_height * sizeof(uint16_t)))
            {
                std::cout << "Can't read length of lines" << std::endl;
                cleanup();
                return false;
            }
        }

        // convert from different endianness
        for(unsigned i = 0; i < m_height * channels; i++)
        {
            m_linesLengths[i] = read_uint16((uint8_t*)&m_linesLengths[i]);
        }
    }

    // only first 3 or 4 channels used
    m_bpp = depth * std::min<unsigned>(channels, 4);
    m_bppImage = depth * channels;

    // we need buffer that can contain one channel data of one
    // row in RLE compressed format. 2*width should be enough
    const unsigned max_line_length = m_width * 2;
    m_buffer = new uint8_t[max_line_length];

    // create separate buffers for each channel (up to 24 buffers by spec)
    for(unsigned i = 0; i < channels; i++)
    {
        m_chBufs[i] = new uint8_t[m_width * m_height];
    }

    // read all channels rgba and extra if available;
    for(unsigned ch = 0; ch < channels; ch++)
    {
        unsigned pos = 0;
        for(unsigned row = 0; row < m_height; row++)
        {
            unsigned lineLength = m_width;
            if(compression == 1)
            {
                lineLength = m_linesLengths[ch * m_height + row];
                if(max_line_length < lineLength)
                {
                    std::cout << "Wrong line length: " << lineLength << std::endl;
                    lineLength = max_line_length;
                }
            }

            size_t readed = file.read(m_buffer, lineLength);
            if(lineLength != readed)
            {
                printf("Error reading Image Data Block\n");
                //cleanup();
                //return false;
            }

            int percent = (int)(100.0f * (ch * m_height + row) / (channels * m_height));
            progress(percent);

            if(compression == 1)
            {
                decompressLine(m_buffer, lineLength, m_chBufs[ch] + pos);
            }
            else
            {
                memcpy(m_chBufs[ch] + pos, m_buffer, m_width);
            }

            pos += m_width;
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
                    const unsigned idx = m_width * y + x;
                    bitmap[0] = *(m_chBufs[0] + idx);
                    bitmap[1] = *(m_chBufs[1] + idx);
                    bitmap[2] = *(m_chBufs[2] + idx);
                    bitmap += 3;
                }
            }
        }
        else
        {
            m_format = GL_RGBA;
            for(unsigned y = 0; y < m_height; y++)
            {
                for(unsigned x = 0; x < m_width; x++)
                {
                    const unsigned idx = m_width * y + x;
                    bitmap[0] = *(m_chBufs[0] + idx);
                    bitmap[1] = *(m_chBufs[1] + idx);
                    bitmap[2] = *(m_chBufs[2] + idx);
                    bitmap[3] = *(m_chBufs[3] + idx);
                    bitmap += 4;
                }
            }
        }
    }
    else if(color_mode == PSD_MODE_CMYK)
    {
        m_format = GL_RGB;
        for(unsigned y = 0; y < m_height; y++)
        {
            for(unsigned x = 0; x < m_width; x++)
            {
                const unsigned idx = m_width * y + x;
                double C = 1.0 - *(m_chBufs[0] + idx) / 255.0; // C
                double M = 1.0 - *(m_chBufs[1] + idx) / 255.0; // M
                double Y = 1.0 - *(m_chBufs[2] + idx) / 255.0; // Y
                double K = 1.0 - *(m_chBufs[3] + idx) / 255.0; // K

                bitmap[0] = (unsigned char)((1.0 - (C * (1.0 - K) + K)) * 255.0);
                bitmap[1] = (unsigned char)((1.0 - (M * (1.0 - K) + K)) * 255.0);
                bitmap[2] = (unsigned char)((1.0 - (Y * (1.0 - K) + K)) * 255.0);
                bitmap += 3;
            }
        }
    }

    cleanup();

    return true;
}

bool CFormatPsd::skipNextBlock(cFile& file)
{
    uint32_t size;
    if(sizeof(uint32_t) != file.read(&size, sizeof(uint32_t)))
    {
        return false;
    }
    size = read_uint32((uint8_t*)&size);
    //std::cout << size << " bytes skipped" << std::endl;
    file.seek(size, SEEK_CUR);

    return true;
}

void CFormatPsd::decompressLine(const uint8_t* src, uint32_t lineLength, uint8_t* dest)
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
                *dest = src[bytes_read];
                dest++;
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
                *dest = next_byte;
                dest++;
            }
        }
    }
}

void CFormatPsd::cleanup()
{
    delete[] m_buffer;
    m_buffer = 0;
    delete[] m_linesLengths;
    m_linesLengths = 0;
    for(unsigned i = 0; i < MAX_CHANNELS; i++)
    {
        delete[] m_chBufs[i];
        m_chBufs[i] = 0;
    }
}

