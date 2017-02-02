/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatpnm.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

namespace
{

    bool readAscii1(cFile& file, sBitmapDescription& desc)
    {
        desc.bpp = 24;
        desc.bppImage = 1;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);

        size_t idx = 0;
        char* line = nullptr;
        size_t len = 0;

        const char* sep = "\n\t ";

        auto out = desc.bitmap.data();
        while (::getline(&line, &len, (FILE*)file.getHandle()) != -1)
        {
            for (auto word = ::strtok(line, sep); word != nullptr; word = ::strtok(nullptr, sep))
            {
                const auto val = (unsigned)::atoi(word) != 0 ? 0 : 255;
                out[idx + 0] = val;
                out[idx + 1] = val;
                out[idx + 2] = val;
                idx += 3;
            }
        }

        return true;
    }

    bool readRaw1(cFile& file, sBitmapDescription& desc)
    {
        desc.bpp = 24;
        desc.bppImage = 1;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);
        const unsigned width = (unsigned)::ceilf(desc.width / 8.0f) * 8;
        std::vector<unsigned char> buffer(width / 8);

        for (unsigned row = 0; row < desc.height; row++)
        {
            if (buffer.size() != file.read(buffer.data(), buffer.size()))
            {
                return false;
            }

            auto out = desc.bitmap.data() + row * desc.pitch;
            unsigned idx = 0;
            for (unsigned i = 0; i < buffer.size(); i++)
            {
                const auto byte = (unsigned char)buffer[i];
                for (unsigned b = 0; b < 8; b++)
                {
                    const unsigned bit = 0x80 >> b;
                    const unsigned char val = (byte & bit) != 0 ? 0 : 255;
                    out[idx + 0] = val;
                    out[idx + 1] = val;
                    out[idx + 2] = val;
                    idx += 3;
                }
            }
        }

        return true;
    }

    bool readAscii8(cFile& file, sBitmapDescription& desc, unsigned maxValue)
    {
        desc.bpp = 24;
        desc.bppImage = 8;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);

        size_t idx = 0;
        char* line = nullptr;
        size_t len = 0;

        const float norm = 255.0f / maxValue;
        const char* sep = "\n\t ";

        auto out = desc.bitmap.data();
        while (::getline(&line, &len, (FILE*)file.getHandle()) != -1)
        {
            for (auto word = ::strtok(line, sep); word != nullptr; word = ::strtok(nullptr, sep))
            {
                const auto val = (unsigned)(::atoi(word) * norm);
                out[idx + 0] = val;
                out[idx + 1] = val;
                out[idx + 2] = val;
                idx += 3;
            }
        }

        return idx == desc.bitmap.size();
    }

    bool readRaw8(cFile& file, sBitmapDescription& desc, unsigned maxValue)
    {
        desc.bpp = 24;
        desc.bppImage = 8;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);
        std::vector<unsigned char> buffer(desc.width);

        const float norm = 255.0f / maxValue;

        for (unsigned row = 0; row < desc.height; row++)
        {
            if (buffer.size() != file.read(buffer.data(), buffer.size()))
            {
                return false;
            }

            auto out = desc.bitmap.data() + row * desc.pitch;
            for (unsigned i = 0; i < desc.width; i++)
            {
                const unsigned idx = i * 3;
                const auto val = (unsigned char)(buffer[i] * norm);
                out[idx + 0] = val;
                out[idx + 1] = val;
                out[idx + 2] = val;
            }
        }

        return true;
    }

    bool readAscii24(cFile& file, sBitmapDescription& desc, unsigned maxValue)
    {
        desc.bpp = desc.bppImage = 24;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);

        size_t idx = 0;
        char* line = nullptr;
        size_t len = 0;

        const float norm = 255.0f / maxValue;
        const char* sep = "\n\t ";

        while (::getline(&line, &len, (FILE*)file.getHandle()) != -1)
        {
            for (auto word = ::strtok(line, sep); word != nullptr; word = ::strtok(nullptr, sep))
            {
                const auto val = (unsigned)(::atoi(word) * norm);
                desc.bitmap[idx++] = val;
            }
        }

        return idx == desc.bitmap.size();
    }

    bool readRaw24(cFile& file, sBitmapDescription& desc, unsigned maxValue)
    {
        desc.bpp = desc.bppImage = 24;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);

        const bool result = desc.bitmap.size() == file.read(desc.bitmap.data(), desc.bitmap.size());

        if (result && maxValue < 255)
        {
            const float norm = 255.0f / maxValue;

            auto out = desc.bitmap.data();
            for (size_t i = 0, size = desc.bitmap.size(); i < size; i++)
            {
                *out *= norm;
            }
        }

        return result;
    }

}

cFormatPnm::cFormatPnm(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatPnm::~cFormatPnm()
{
}

bool cFormatPnm::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    bool result = false;
    char* line = nullptr;
    size_t len = 0;
    ssize_t read;

    unsigned format = 0;
    unsigned maxValue = 0;

    enum class Token
    {
        Format,
        Width,
        Height,
        MaxValue,
        Data,
    };
    Token token = Token::Format;

    while (token != Token::Data)
    {
        if ((read = ::getline(&line, &len, (FILE*)file.getHandle())) != -1)
        {
            if (line[0] != '#')
            {
                const char* sep = "\n\t ";
                for (auto word = ::strtok(line, sep); word != nullptr && token != Token::Data; word = ::strtok(nullptr, sep))
                {
                    const auto len = ::strlen(word);
                    switch (token)
                    {
                    case Token::Format:
                        if (len >= 2 && word[0] == 'P')
                        {
                            token = Token::Width;
                            format = word[1] - '0';
                        }
                        break;

                    case Token::Width:
                        token = Token::Height;
                        desc.width = (unsigned)::atoi(word);
                        break;

                    case Token::Height:
                        token = (format != 4 && format != 1) ? Token::MaxValue : Token::Data;
                        desc.height = (unsigned)::atoi(word);
                        break;

                    case Token::MaxValue:
                        token = Token::Data;
                        maxValue = (unsigned)::atoi(word);
                        break;

                    case Token::Data: // prevent compiler warning
                        break;
                    }
                }
            }
        }
    }
    ::free(line);

    ::printf("format: %u\n", format);
    ::printf("size: %u x %u\n", desc.width, desc.height);

    switch (format)
    {
    case 1: // 1-ascii
        m_formatName = "pnm/1-acii";
        result = readAscii1(file, desc);
        break;

    case 4: // 1-raw
        m_formatName = "pnm/1-raw";
        result = readRaw1(file, desc);
        break;

    case 2: // 8-ascii
        m_formatName = "pnm/8-acii";
        result = readAscii8(file, desc, maxValue);
        break;

    case 5: // 8-raw
        m_formatName = "pnm/8-raw";
        result = readRaw8(file, desc, maxValue);
        break;

    case 3: // 24-ascii
        m_formatName = "pnm/24-acii";
        result = readAscii24(file, desc, maxValue);
        break;

    case 6: // 24-raw
        m_formatName = "pnm/24-raw";
        result = readRaw24(file, desc, maxValue);
        break;
    }

    return result;
}
