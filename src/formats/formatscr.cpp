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
            // palette PULSAR (0xcd)
            static sColor Colors[] =
            {
                // normal
                { 0x00, 0x00, 0x00 },
                { 0x00, 0x00, 0xcd },
                { 0xcd, 0x00, 0x00 },
                { 0xcd, 0x00, 0xcd },
                { 0x00, 0xcd, 0x00 },
                { 0x00, 0xcd, 0xcd },
                { 0xcd, 0xcd, 0x00 },
                { 0xcd, 0xcd, 0xcd },

                // bright
                { 0x00, 0x00, 0x00 },
                { 0x00, 0x00, 0xff },
                { 0xff, 0x00, 0x00 },
                { 0xff, 0x00, 0xff },
                { 0x00, 0xff, 0x00 },
                { 0x00, 0xff, 0xff },
                { 0xff, 0xff, 0x00 },
                { 0xff, 0xff, 0xff },
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

    void fillThird(unsigned layer, const unsigned char* buffer, sBitmapDescription& desc, unsigned X, sPixelRGB* out)
    {
        const unsigned char* zxPixels = buffer + 2048 * layer;
        const unsigned char* zxColors = buffer + 6144 + 2048 / X * layer;

        for (unsigned y = 0; y < 64; y++)
        {
            const unsigned line = (y * 8) % 64 + (y * 8) / 64;
            auto startLine = &out[line * desc.width];
            for (unsigned x = 0; x < 256 / 8; x++)
            {
                const unsigned char pixels = *zxPixels++;
                const unsigned char attribute = zxColors[(line / X) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
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
        // BMc1,
        // BMc2,
        BMc4,
        // Chr,

        Unknown
    };

    struct ScreenProperty
    {
        unsigned size;
        unsigned w;
        unsigned h;
        Type type;
        const char* formatName;
    };

    ScreenProperty getType(unsigned size)
    {
        const ScreenProperty sizesList[] =
        {
            {  6912, 256, 192, Type::Scr,      "zx-scr"  },
            { 11136, 384, 304, Type::ScB,      "zx-bsc"  },
            {   768, 256, 192, Type::Atr,      "zx-atr"  },
            {  7680, 256, 192, Type::Mc4,      "zx-mc4"  },
            {  9216, 256, 192, Type::Mc2,      "zx-mc2"  },
            { 12288, 256, 192, Type::Mc1,      "zx-mc1"  },
            // { 99999, 384, 304, Type::BMc1,     "zx-bmc1" },
            // { 99999, 384, 304, Type::BMc2,     "zx-bmc2" },
            { 11904, 384, 304, Type::BMc4,     "zx-bmc4" },
            // { 41479, 256, 192, Type::Chr,      "zx-chr$" },
        };

        for (size_t i = 0; i < sizeof(sizesList) / sizeof(sizesList[0]); i++)
        {
            const auto& s = sizesList[i];
            if (s.size == size)
            {
                return s;
            }
        }

        return { 0, 0, 0, Type::Unknown,  "" };
    }

}

void cFormatScr::loadScr(const unsigned char* buffer, sBitmapDescription& desc) const
{
    for (unsigned i = 0; i < 3; i++)
    {
        auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i);
        fillThird(i, buffer, desc, 8, out);
    }
}

void cFormatScr::loadScB(const unsigned char* buffer, sBitmapDescription& desc) const
{
    for (unsigned i = 0; i < 3; i++)
    {
        const unsigned top = 64 * desc.pitch; //56 * desc.pitch;
        const unsigned left = 64 * 3;
        auto out = (sPixelRGB*)(desc.bitmap.data() + top + left + desc.pitch * 64 * i);
        fillThird(i, buffer, desc, 8, out);
    }
}

void cFormatScr::loadAtr(const unsigned char* buffer, sBitmapDescription& desc) const
{
    const unsigned char px[2] = { 0b01010101, 0b10101010 };

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
    if (X == 1)
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
    else
    {
        for (unsigned i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i);
            fillThird(i, buffer, desc, X, out);
        }
    }
}

void cFormatScr::loadBMcX(const unsigned char* buffer, sBitmapDescription& desc, unsigned X) const
{
    for (unsigned i = 0; i < 3; i++)
    {
        const unsigned top = 64 * desc.pitch; //56 * desc.pitch;
        const unsigned left = 64 * 3;
        auto out = (sPixelRGB*)(desc.bitmap.data() + top + left + desc.pitch * 64 * i);

        if (0)
        {
            fillThird(i, buffer, desc, X, out);
        }
        else
        {
            const unsigned char* zxPixels = buffer + 2048 * i;
            const unsigned char* zxColors = buffer + 6144 + 2048 / X * i;

            for (unsigned y = 0; y < 64; y++)
            {
                const unsigned line = (y * 8) % 64 + (y * 8) / 64;
                auto startLine = &out[line * 384];
                for (unsigned x = 0; x < 256 / 8; x++)
                {
                    const unsigned char pixels = *zxPixels++;
                    // const unsigned char attribute = zxColors[((i * 64 + y) / X) * 32 + x];
                    // const unsigned char attribute = zxColors[(y % 8) * 32 + x];
                    const unsigned char attribute = zxColors[(line / 4) * 32 + x];
                    putEightPixels(&startLine[x * 8], pixels, attribute);
                }
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
    const auto screen = getType(size);
    if (screen.type == Type::Unknown)
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

    desc.width    = screen.w;
    desc.height   = screen.h;
    desc.pitch    = desc.width * 3;
    desc.bitmap.resize(desc.pitch * desc.height);

    m_formatName = screen.formatName;

    switch (screen.type)
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
