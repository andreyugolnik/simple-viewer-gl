/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatscr.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <cstdint>

cFormatScr::cFormatScr(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

cFormatScr::~cFormatScr()
{
}

namespace
{

    struct sPixelRGB
    {
        struct sColor
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };

        void set(bool isSet, unsigned char attribute)
        {
            static sColor Colors[] =
            {
                // normal
                { 0, 0, 0 },
                { 0, 0, 192 },
                { 192, 0, 0 },
                { 192, 0, 192 },
                { 0, 192, 0 },
                { 0, 192, 192 },
                { 192, 192, 0 },
                { 192, 192, 192 },

                // bright
                { 0, 0, 0 },
                { 0, 0, 255 },
                { 255, 0, 0 },
                { 255, 0, 255 },
                { 0, 255, 0 },
                { 0, 255, 255 },
                { 255, 255, 0 },
                { 255, 255, 255 },
            };

            const unsigned bright = (attribute & 0x40) >> 3;
            if (!isSet)
            {
                attribute >>= 3;
            }
            color = Colors[bright + (attribute & 0x7)];
        }

        sColor color;
    };

    void putEightPixels(sPixelRGB* out, const unsigned char pixels, const unsigned char attribute)
    {
        for (unsigned i = 0; i < 8; i++)
        {
            const unsigned bit = 0x80 >> i;
            const bool isSet = pixels & bit;
            out->set(isSet, attribute);
            out++;
        }
    }

    void fillThird(const unsigned char* zxPixels, const unsigned char* zxColors, sPixelRGB* out)
    {
        for (unsigned y = 0; y < 64; y++)
        {
            const unsigned line = (y * 8) % 64 + (y * 8) / 64;
            auto startLine = &out[line * 256];
            for (unsigned x = 0; x < 256 / 8; x++)
            {
                const unsigned char pixels = *zxPixels;
                const unsigned char attribute = zxColors[(y % 8) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
                zxPixels++;
            }
        }
    }

    enum class Type : unsigned
    {
        Scr, // done
        ScB, // partial
        Atr, // done
        Mc4, // done
        Mc2, // done
        Mc1, // done
        BMc4,
        Chr,

        Unknown
    };

    struct SizeType
    {
        unsigned size;
        Type type;
    };

    Type getType(unsigned size)
    {
        const SizeType sizesList[] =
        {
            {  6912, Type::Scr  },
            { 11136, Type::ScB  },
            {   768, Type::Atr  },
            {  7680, Type::Mc4  },
            {  9216, Type::Mc2  },
            { 12288, Type::Mc1  },
            { 11904, Type::BMc4 },
            { 41479, Type::Chr  },
        };

        for (size_t i = 0; i < sizeof(sizesList)/sizeof(sizesList[0]); i++)
        {
            const auto& s = sizesList[i];
            if (s.size == size)
            {
                return s.type;
            }
        }

        return Type::Unknown;
    }

}

void cFormatScr::loadScr(const unsigned char* buffer, sBitmapDescription& desc) const
{
    for (unsigned i = 0; i < 3; i++)
    {
        const unsigned char* zxPixels = buffer + 2048 * i;
        const unsigned char* zxColors = buffer + 6144 + 256 * i;
        auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i);
        fillThird(zxPixels, zxColors, out);
    }
}

void cFormatScr::loadScB(const unsigned char* buffer, sBitmapDescription& desc) const
{
    desc.width  = 384;
    desc.height = 304;
    desc.pitch = desc.width * 3;
    desc.bitmap.resize(desc.pitch * desc.height);

    for (unsigned i = 0; i < 3; i++)
    {
        const unsigned char* zxPixels = buffer + 2048 * i;
        const unsigned char* zxColors = buffer + 6144 + 256 * i;
        auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i + desc.pitch * 64);

        for (unsigned y = 0; y < 64; y++)
        {
            const unsigned line = (y * 8) % 64 + (y * 8) / 64;
            auto startLine = &out[64 + line * 384];
            for (unsigned x = 0; x < 256 / 8; x++)
            {
                const unsigned char pixels = *zxPixels;
                const unsigned char attribute = zxColors[(y % 8) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
                zxPixels++;
            }
        }
    }
}

void cFormatScr::loadAtr(const unsigned char* buffer, sBitmapDescription& desc) const
{
    const unsigned char px[2] = { 0b10101010, 0b01010101 };

    const unsigned char* zxColors = buffer;
    auto out = (sPixelRGB*)desc.bitmap.data();

    for (unsigned y = 0; y < 192; y++)
    {
        auto startLine = &out[y * 256];
        for (unsigned x = 0; x < 256 / 8; x++)
        {
            const unsigned char pixels = px[y % 2];
            const unsigned char attribute = zxColors[(y / 8) * 32 + x];
            putEightPixels(&startLine[x * 8], pixels, attribute);
        }
    }
}

void cFormatScr::loadMcX(const unsigned char* buffer, sBitmapDescription& desc, unsigned X) const
{
    const unsigned char* zxPixels = buffer;
    const unsigned char* zxColors = buffer + 6144;
    auto out = (sPixelRGB*)desc.bitmap.data();

    for (unsigned y = 0; y < 192; y++)
    {
        auto startLine = &out[y * 256];
        for (unsigned x = 0; x < 256 / 8; x++)
        {
            const unsigned char pixels = *zxPixels++;
            const unsigned char attribute = zxColors[(y / X) * 32 + x];
            putEightPixels(&startLine[x * 8], pixels, attribute);
        }
    }
}

void cFormatScr::loadBMcX(const unsigned char* buffer, sBitmapDescription& desc, unsigned X) const
{
    desc.width  = 384;
    desc.height = 304;
    desc.pitch = desc.width * 3;
    desc.bitmap.resize(desc.pitch * desc.height);

    for (unsigned i = 0; i < 3; i++)
    {
        const unsigned char* zxPixels = buffer + 2048 * i;
        const unsigned char* zxColors = buffer + 6144;
        auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i + desc.pitch * 64);

        for (unsigned y = 0; y < 64; y++)
        {
            const unsigned line = (y * 8) % 64 + (y * 8) / 64;
            auto startLine = &out[64 + line * 384];
            for (unsigned x = 0; x < 256 / 8; x++)
            {
                const unsigned char pixels = *zxPixels;
                // const unsigned char attribute = zxColors[((i * 64 + y) / X) * 32 + x];
                const unsigned char attribute = zxColors[(y % 8) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
                zxPixels++;
            }
        }
    }
}

bool cFormatScr::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    const unsigned size = file.getSize();
    const auto type = getType(size);
    if (type == Type::Unknown)
    {
        printf("Not a ZX-Spectrum screen\n");
        return false;
    }

    std::vector<unsigned char> buffer(size);
    if (file.read(buffer.data(), size) != size)
    {
        printf("Can't read ZX-Spectrum screen data\n");
        return false;
    }

    desc.size     = size;
    desc.bppImage = 1;
    desc.bpp      = 24;
    desc.format   = GL_RGB;

    desc.width    = 256;
    desc.height   = 192;
    desc.pitch    = desc.width * 3;
    desc.bitmap.resize(desc.pitch * desc.height);

    switch (type)
    {
    case Type::Scr:
        loadScr(buffer.data(), desc);
        break;

    case Type::ScB:
        loadScB(buffer.data(), desc);
        break;

    case Type::Atr:
        loadAtr(buffer.data(), desc);
        break;

    case Type::Mc4:
        loadMcX(buffer.data(), desc, 4);
        break;

    case Type::Mc2:
        loadMcX(buffer.data(), desc, 2);
        break;

    case Type::Mc1:
        loadMcX(buffer.data(), desc, 1);
        break;

    case Type::BMc4:
        loadBMcX(buffer.data(), desc, 4);
        break;

    case Type::Unknown: // prevent compiler warning
        break;
    }

    return true;
}
