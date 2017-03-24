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
#include "../common/helpers.h"
#include "../types/types.h"

#include <cstdio>
#include <cstring>

cFormatScr::cFormatScr(iCallbacks* callbacks) : cFormat(callbacks)
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
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };

        void set(bool isSet, uint8_t attribute)
        {
            // palette PULSAR (0xcd)
            static sColor Colors[] = {
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

            const uint32_t bright = (attribute & 0x40) >> 3;
            if (!isSet)
            {
                attribute >>= 3;
            }
            color = Colors[bright + (attribute & 0x07)];
        }

        sColor color;
    };

    void putSixteenPixels(sPixelRGB* out, uint8_t color)
    {
        const uint8_t left = color & 0x07;
        for (uint32_t i = 0; i < 8; i++)
        {
            out->set(true, left);
            out++;
        }

        const uint8_t right = (color >> 3) & 0x07;
        for (uint32_t i = 0; i < 8; i++)
        {
            out->set(true, right);
            out++;
        }
    }

    void makeBorder(const uint8_t* zxBorder, sBitmapDescription& desc)
    {
        // top
        for (uint32_t y = 0; y < 64; y++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + y * desc.pitch);
            for (uint32_t x = 0; x < 24; x++)
            {
                const uint8_t color = *zxBorder++;
                putSixteenPixels(out, color);
                out += 16;
            }
        }

        // left / right
        for (uint32_t y = 0; y < 192; y++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + (y + 64) * desc.pitch);
            for (uint32_t x = 0; x < 4; x++)
            {
                const uint8_t color = *zxBorder++;
                putSixteenPixels(out, color);
                out += 16;
            }

            out += 256;
            for (uint32_t x = 0; x < 4; x++)
            {
                const uint8_t color = *zxBorder++;
                putSixteenPixels(out, color);
                out += 16;
            }
        }

        // bottom
        for (uint32_t y = 0; y < 48; y++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + (y + 64 + 192) * desc.pitch);
            for (uint32_t x = 0; x < 24; x++)
            {
                const uint8_t color = *zxBorder++;
                putSixteenPixels(out, color);
                out += 16;
            }
        }
    }

    void putEightPixels(sPixelRGB* out, const uint8_t pixels, const uint8_t attribute)
    {
        for (uint32_t i = 0; i < 8; i++)
        {
            const uint32_t bit = 0x80 >> i;
            const bool isSet = pixels & bit;
            out->set(isSet, attribute);
            out++;
        }
    }

    void fillThird(uint32_t layer, const uint8_t* buffer, sBitmapDescription& desc, uint32_t X,
                   sPixelRGB* out)
    {
        const uint8_t* zxPixels = buffer + 2048 * layer;
        const uint8_t* zxColors = buffer + 6144 + 2048 / X * layer;

        for (uint32_t y = 0; y < 64; y++)
        {
            const uint32_t line = (y * 8) % 64 + (y * 8) / 64;
            auto startLine = &out[line * desc.width];
            for (uint32_t x = 0; x < 256 / 8; x++)
            {
                const uint8_t pixels = *zxPixels++;
                const uint8_t attribute = zxColors[(line / X) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
            }
        }
    }

    struct ScrDescription
    {
        uint32_t size;
        uint32_t w;
        uint32_t h;

        enum class Type : uint32_t
        {
            Scr, // done
            ScS, // done
            ScB, // partial
            Atr, // done
            Mc4, // done
            Mc2, // done
            Mc1, // done
            BMc4,
            // Chr,

            Unknown
        };

        Type type;

        const char* formatName;
    };

    ScrDescription getType(uint32_t size)
    {
        const ScrDescription sizesList[] = {
            { 6912, 256, 192, ScrDescription::Type::Scr, "zx-scr" },
            { 6929, 256, 192, ScrDescription::Type::ScS, "zx-scr$" },
            { 11136, 384, 304, ScrDescription::Type::ScB, "zx-bsc" },
            { 768, 256, 192, ScrDescription::Type::Atr, "zx-atr" },
            { 7680, 256, 192, ScrDescription::Type::Mc4, "zx-mc4" },
            { 9216, 256, 192, ScrDescription::Type::Mc2, "zx-mc2" },
            { 12288, 256, 192, ScrDescription::Type::Mc1, "zx-mc1" },
            { 11904, 384, 304, ScrDescription::Type::BMc4, "zx-bmc4" },
            // { 41479, 256, 192, ScrDescription::Type::Chr, "zx-chr$" },
        };

        for (size_t i = 0; i < helpers::countof(sizesList); i++)
        {
            const auto& s = sizesList[i];
            if (s.size == size)
            {
                return s;
            }
        }

        return { 0, 0, 0, ScrDescription::Type::Unknown, "" };
    }

    void loadScr(const uint8_t* buffer, sBitmapDescription& desc)
    {
        for (uint32_t i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i);
            fillThird(i, buffer, desc, 8, out);
        }
    }

    void loadScB(const uint8_t* buffer, sBitmapDescription& desc)
    {
        const uint32_t top = 64;
        const uint32_t left = 64;
        for (uint32_t i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + (top + i * 64) * desc.pitch) + left;
            fillThird(i, buffer, desc, 8, out);
        }

        const uint8_t* zxBorder = buffer + 6144 + 768;
        makeBorder(zxBorder, desc);
    }

    void loadAtr(const uint8_t* buffer, sBitmapDescription& desc)
    {
        const uint8_t px[2] = { 0x55, 0xaa }; // { 0b01010101, 0b10101010 };

        const uint8_t* zxColors = buffer;
        auto out = (sPixelRGB*)desc.bitmap.data();

        for (uint32_t y = 0; y < 192; y++)
        {
            auto startLine = &out[y * 256];
            for (uint32_t x = 0; x < 256 / 8; x++)
            {
                const uint8_t pixels = px[y % 2];
                const uint8_t attribute = zxColors[(y / 8) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
            }
        }
    }

    void loadMcX(const uint8_t* buffer, sBitmapDescription& desc, const ScrDescription& prop)
    {
        if (prop.type == ScrDescription::Type::Mc1)
        {
            const uint32_t X = 1;

            const uint8_t* zxPixels = buffer;
            const uint8_t* zxColors = buffer + 6144;
            auto out = (sPixelRGB*)desc.bitmap.data();

            for (uint32_t y = 0; y < 192; y++)
            {
                auto startLine = &out[y * 256];
                for (uint32_t x = 0; x < 256 / 8; x++)
                {
                    const uint8_t pixels = *zxPixels++;
                    const uint8_t attribute = zxColors[(y / X) * 32 + x];
                    putEightPixels(&startLine[x * 8], pixels, attribute);
                }
            }
        }
        else
        {
            const uint32_t X = prop.type == ScrDescription::Type::Mc4 ? 4 : 2;

            for (uint32_t i = 0; i < 3; i++)
            {
                auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i);
                fillThird(i, buffer, desc, X, out);
            }
        }
    }

    void loadBMc4(const uint8_t* buffer, sBitmapDescription& desc, const ScrDescription& prop)
    {
        if (prop.type == ScrDescription::Type::BMc4)
        {
            const uint32_t top = 64;
            const uint32_t left = 64;

            for (uint32_t i = 0; i < 3; i++)
            {
                auto out = (sPixelRGB*)(desc.bitmap.data() + ((top + 64 * i) * desc.pitch)) + left;
                auto zxPixels = buffer + 2048 * i;

                auto zxColors0 = buffer + 6144 + (768 / 3) * i;
                auto zxColors1 = buffer + 6144 + 768 + (768 / 3) * i;

                for (uint32_t y = 0; y < 64; y++)
                {
                    const uint32_t line = (y * 8) % 64 + (y * 8) / 64;
                    auto startLine = &out[line * desc.width];
                    auto zxColors = ((line % 8) < 4 ? zxColors0 : zxColors1) + (line / 8) * 32;
                    for (uint32_t x = 0; x < 256 / 8; x++)
                    {
                        const uint8_t pixels = *zxPixels++;
                        const uint8_t attribute = *zxColors++;
                        putEightPixels(&startLine[x * 8], pixels, attribute);
                    }
                }
            }

            const uint8_t* zxBorder = buffer + 6144 + 768 * 2;
            makeBorder(zxBorder, desc);
        }
    }
}

bool cFormatScr::isSupported(cFile& file, Buffer& /*buffer*/) const
{
    const auto screen = getType(file.getSize());
    return screen.type != ScrDescription::Type::Unknown;
}

bool cFormatScr::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    const uint32_t size = file.getSize();
    const auto screen = getType(size);
    if (screen.type == ScrDescription::Type::Unknown)
    {
        ::printf("(EE) Not a ZX-Spectrum screen.\n");
        return false;
    }

    std::vector<uint8_t> buffer(size);
    if (file.read(buffer.data(), size) != size)
    {
        ::printf("(EE) Can't read ZX-Spectrum screen data.\n");
        return false;
    }

    desc.size = size;
    desc.bppImage = 1;
    desc.bpp = 24;
    desc.format = GL_RGB;

    desc.width = screen.w;
    desc.height = screen.h;
    desc.pitch = desc.width * 3;
    desc.bitmap.resize(desc.pitch * desc.height);

    m_formatName = screen.formatName;

    switch (screen.type)
    {
    case ScrDescription::Type::Scr:
        loadScr(buffer.data(), desc);
        break;

    case ScrDescription::Type::ScS:
        desc.exifList.push_back({ "Comment", (const char*)buffer.data() });
        loadScr(buffer.data() + 17, desc);
        break;

    case ScrDescription::Type::ScB:
        loadScB(buffer.data(), desc);
        break;

    case ScrDescription::Type::Atr:
        loadAtr(buffer.data(), desc);
        break;

    case ScrDescription::Type::Mc4:
        loadMcX(buffer.data(), desc, screen);
        break;

    case ScrDescription::Type::Mc2:
        loadMcX(buffer.data(), desc, screen);
        break;

    case ScrDescription::Type::Mc1:
        loadMcX(buffer.data(), desc, screen);
        break;

    case ScrDescription::Type::BMc4:
        loadBMc4(buffer.data(), desc, screen);
        break;

    case ScrDescription::Type::Unknown: // prevent compiler warning
        break;
    }

    return true;
}
