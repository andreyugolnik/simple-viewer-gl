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
#include "../common/helpers.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

namespace
{

    bool isEndLine(char ch)
    {
        const char delims[] = "\n\0";

        for (unsigned i = 0; i < helpers::countof(delims); i++)
        {
            if (delims[i] == ch)
            {
                return true;
            }
        }
        return false;
    }

    typedef std::vector<char> Line;

    bool getline(Line& line, cFile& file)
    {
        line.clear();

        auto remain = file.getSize() - file.getOffset();
        if (remain == 0)
        {
            // ::printf("- eof\n");
            return false;
        }

        const unsigned bufferSize = 20;
        unsigned bufferOffset = 0;

        while (remain != 0)
        {
            auto offset = file.getOffset();

            const auto size = std::min<unsigned>(remain, bufferSize);
            line.resize(line.size() + size);

            auto buffer = line.data() + bufferOffset;
            if (file.read(buffer, size) != size)
            {
                line.push_back(0);
                // ::printf("- error read file\n");
                return true;
            }

            for (unsigned i = 0; i < size; i++)
            {
                auto ch = buffer[i];
                if (isEndLine(ch))
                {
                    offset += i + (ch == 0 ? 0 : 1);
                    line.resize(bufferOffset + i + 1);
                    line[line.size() - 1] = 0;
                    file.seek(offset, SEEK_SET);
                    // ::printf("- stop char detected, seek to: %u\n", (unsigned)offset);
                    return true;
                }
            }

            remain -= size;
            bufferOffset += size;
        }

        return true;
    }

    const char* TokenSep = "\n\t ";

    bool readAscii1(cFile& file, sBitmapDescription& desc)
    {
        desc.format = GL_LUMINANCE;
        desc.bpp = 8;
        desc.bppImage = 1;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);

        size_t idx = 0;
        Line line;

        auto out = desc.bitmap.data();
        while (getline(line, file))
        {
            for (auto word = ::strtok(line.data(), TokenSep); word != nullptr; word = ::strtok(nullptr, TokenSep))
            {
                const auto val = (unsigned)::atoi(word) != 0 ? 0 : 255;
                out[idx++] = val;
            }
        }

        return true;
    }

    bool readRaw1(cFile& file, sBitmapDescription& desc)
    {
        desc.format = GL_LUMINANCE;
        desc.bpp = 8;
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
            size_t idx = 0;
            for (unsigned i = 0; i < buffer.size(); i++)
            {
                const auto byte = (unsigned char)buffer[i];
                for (unsigned b = 0; b < 8; b++)
                {
                    const unsigned bit = 0x80 >> b;
                    const unsigned char val = (byte & bit) != 0 ? 0 : 255;
                    out[idx++] = val;
                }
            }
        }

        return true;
    }

    bool readAscii8(cFile& file, sBitmapDescription& desc, unsigned maxValue)
    {
        desc.format = GL_LUMINANCE;
        desc.bpp = 8;
        desc.bppImage = 8;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);

        size_t idx = 0;
        Line line;

        const float norm = 255.0f / maxValue;

        auto out = desc.bitmap.data();
        while (getline(line, file))
        {
            for (auto word = ::strtok(line.data(), TokenSep); word != nullptr; word = ::strtok(nullptr, TokenSep))
            {
                const auto val = (unsigned)(::atoi(word) * norm);
                out[idx++] = val;
            }
        }

        return idx == desc.bitmap.size();
    }

    bool readRaw8(cFile& file, sBitmapDescription& desc, unsigned maxValue)
    {
        desc.format = GL_LUMINANCE;
        desc.bpp = 8;
        desc.bppImage = 8;
        desc.pitch = desc.width * desc.bpp / 8;
        desc.bitmap.resize(desc.pitch * desc.height);
        std::vector<unsigned char> buffer(desc.width);

        const float norm = 255.0f / maxValue;

        for (unsigned row = 0; row < desc.height; row++)
        {
            file.read(buffer.data(), buffer.size());

            auto out = desc.bitmap.data() + row * desc.pitch;
            for (unsigned i = 0; i < desc.width; i++)
            {
                const auto val = (unsigned char)(buffer[i] * norm);
                out[i] = val;
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
        Line line;

        const float norm = 255.0f / maxValue;

        while (getline(line, file))
        {
            for (auto word = ::strtok(line.data(), TokenSep); word != nullptr; word = ::strtok(nullptr, TokenSep))
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

bool cFormatPnm::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, 2))
    {
        return false;
    }

    const auto h = reinterpret_cast<const char*>(buffer.data());
    return h[0] == 'P' && h[1] >= '1' && h[1] <= '6' && file.getSize() >= 8;
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
    Line line;

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
        if (getline(line, file))
        {
            if (line.size() == 0 || line[0] == '#')
            {
                continue;
            }

            auto begin = line.data();
            const char* word = nullptr;
            while ((word = ::strsep(&begin, TokenSep)) != nullptr && token != Token::Data)
            {
                const auto wordLen = ::strlen(word);
                if (wordLen == 0)
                {
                    continue;
                }

                // ::printf("token: '%s'\n", word);

                switch (token)
                {
                case Token::Format:
                    if (wordLen >= 2 && word[0] == 'P')
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

                case Token::Data: // do nothing
                    break;
                }
            }
        }
    }

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
