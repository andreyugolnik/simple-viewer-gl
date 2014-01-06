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

CFormatPsd::CFormatPsd(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
    , m_buffer(0)
    , m_linesLengths(0)
{
    memset(m_chBufs, 0, sizeof(m_chBufs));
}

CFormatPsd::~CFormatPsd()
{
}

bool CFormatPsd::Load(const char* filename, unsigned subImage)
{
    if(openFile(filename) == false)
    {
        return false;
    }

    PSD_HEADER header;
    if(sizeof(PSD_HEADER) != fread(&header, 1, sizeof(PSD_HEADER), m_file))
    {
        std::cout << "Can't read PSD header" << std::endl;
        reset();
        return false;
    }

    if(header.signature[0] != '8' || header.signature[1] != 'B' || header.signature[2] != 'P' || header.signature[3] != 'S')
    {
        std::cout << "Not valid PSD file" << std::endl;
        reset();
        return false;
    }

    int color_mode = read_uint16((uint8_t*)&header.color_mode);
    if(color_mode != PSD_MODE_RGB && color_mode != PSD_MODE_CMYK)
    {
        std::cout << "Unsupported color mode: " << color_mode << std::endl;
        reset();
        return false;
    }

    int depth = read_uint16((uint8_t*)&header.depth);
    if(depth != 8)
    {
        std::cout << "Unsupported depth: " << depth << std::endl;
        reset();
        return false;
    }

    unsigned channels = read_uint16((uint8_t*)&header.channels);
    if(channels != 3 && channels != 4)
    {
        std::cout << "Unsupported cannels count: " << channels << std::endl;
        //reset();
        //return false;
    }
    const unsigned extraChannels = channels - std::min<unsigned>(channels, 4);
    std::cout << " " << extraChannels << " extra channel(s),";

    // skip Color Mode Data Block
    if(false == skipNextBlock())
    {
        std::cout << "Can't read Color Mode Data Block" << std::endl;
        reset();
        return false;
    }

    // skip Image Resources Block
    if(false == skipNextBlock())
    {
        std::cout << "Can't read Image Resources Block" << std::endl;
        reset();
        return false;
    }

    // Layer and Mask Information Block
    if(false == skipNextBlock())
    {
        std::cout << "Can't read Layer and Mask Information Block" << std::endl;
        reset();
        return false;
    }

    // Image Data Block
    uint16_t compression;
    if(sizeof(uint16_t) != fread(&compression, 1, sizeof(uint16_t), m_file))
    {
        std::cout << "Can't read compression info" << std::endl;
        reset();
        return false;
    }
    compression = read_uint16((uint8_t*)&compression);

    m_width = read_uint32((uint8_t*)&header.columns);
    m_height = read_uint32((uint8_t*)&header.rows);

    // this will be needed for RLE decompression
    m_linesLengths = new uint16_t[channels * m_height];
    for(unsigned i = 0; i < channels; i++)
    {
        const unsigned pos = m_height * i;

        if(m_height * sizeof(uint16_t) != fread(&m_linesLengths[pos], 1, m_height * sizeof(uint16_t), m_file))
        {
            std::cout << "Can't read length of lines" << std::endl;
            cleanup();
            return false;
        }

        // convert from different endianness
        for(unsigned a = 0; a < m_height; a++)
        {
            m_linesLengths[pos + a] = read_uint16((uint8_t*)&m_linesLengths[pos + a]);
        }
    }

    // !!!! this is a temporal hack, need more investigation !!!
    // read only first 3 or 4 channels
    channels = std::min<unsigned>(channels, 4);

    m_bpp = depth * channels;
    m_bppImage = m_bpp;

    // we need buffer that can contain one channel data of one
    // row in RLE compressed format. 2*width should be enough
    m_buffer = new uint8_t[m_width * 2];

    // create separate buffers for each channel (up to 24 buffers by spec)
    for(unsigned i = 0; i < channels; i++)
    {
        m_chBufs[i] = new uint8_t[m_width * m_height];
    }

    // read all channels rgba and extra if available;
    unsigned currentRow = 0;
    unsigned currentChannel = 0;
    unsigned pos = 0;
    bool done = false;
    do
    {
        unsigned lineLength = m_width;
        if(compression == 1)
        {
            lineLength = m_linesLengths[currentChannel * m_height + currentRow];
        }

        unsigned readed = fread(m_buffer, 1, lineLength, m_file);
        if(m_width * 2 < lineLength)
        {
            std::cout << "Wrong line length: " << lineLength << std::endl;
        }
        if(lineLength != readed)
        {
            std::cout << "Error reading Image Data Block" << std::endl;
            cleanup();
            return false;
        }

        int percent = (int)(100.0f * (currentChannel * m_height + currentRow) / (channels * m_height));
        progress(percent);

        if(compression == 1)
        {
            decompressLine(m_buffer, lineLength, m_chBufs[currentChannel] + pos);
        }
        else
        {
            memcpy(m_chBufs[currentChannel] + pos, m_buffer, m_width);
        }

        pos += m_width;
        currentRow++;

        if(currentRow == m_height)
        {
            currentRow = 0;
            pos = 0;
            currentChannel++;
            if(currentChannel == channels)
            {
                done = true;
            }
        }
    } while(done == false);

    // convert or copy channel buffers to RGB / RGBA
    m_pitch = m_width * channels;
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
                    bitmap[0] = *(m_chBufs[0] + m_width * y + x);
                    bitmap[1] = *(m_chBufs[1] + m_width * y + x);
                    bitmap[2] = *(m_chBufs[2] + m_width * y + x);
                    bitmap += 3;
                }
            }
        }
        else if(channels == 4)
        {
            m_format = GL_RGBA;
            for(unsigned y = 0; y < m_height; y++)
            {
                for(unsigned x = 0; x < m_width; x++)
                {
                    bitmap[0] = *(m_chBufs[0] + m_width * y + x);
                    bitmap[1] = *(m_chBufs[1] + m_width * y + x);
                    bitmap[2] = *(m_chBufs[2] + m_width * y + x);
                    bitmap[3] = *(m_chBufs[3] + m_width * y + x);
                    bitmap += 4;
                }
            }
        }
        else
        {
            std::cout << "Should't be happened" << std::endl;
        }
    }
    else if(color_mode == PSD_MODE_CMYK)
    {
        m_format = GL_RGB;
        for(unsigned y = 0; y < m_height; y++)
        {
            for(unsigned x = 0; x < m_width; x++)
            {
                double C = 1.0 - *(m_chBufs[0] + m_width * y + x) / 255.0;	// C
                double M = 1.0 - *(m_chBufs[1] + m_width * y + x) / 255.0;	// M
                double Y = 1.0 - *(m_chBufs[2] + m_width * y + x) / 255.0;	// Y
                double K = 1.0 - *(m_chBufs[3] + m_width * y + x) / 255.0;	// K

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

bool CFormatPsd::skipNextBlock()
{
    uint32_t size;
    if(sizeof(uint32_t) != fread(&size, 1, sizeof(uint32_t), m_file))
    {
        return false;
    }
    size = read_uint32((uint8_t*)&size);
    //	std::cout << size << " bytes skipped" << std::endl;
    fseek(m_file, size, SEEK_CUR);

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
    if(m_file != 0)
    {
        fclose(m_file);
        m_file = 0;
    }
}

