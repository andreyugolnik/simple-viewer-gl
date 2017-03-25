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

cFormatScr::cFormatScr(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatScr::~cFormatScr()
{
}

namespace
{
    struct sColor
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    // palette PULSAR (0xcd)
    static const sColor Palette[] =
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

    struct sPixelRGB
    {
        void set(bool isSet, uint8_t attribute)
        {
            const uint32_t bright = (attribute & 0x40) >> 3;
            if (isSet == false)
            {
                attribute >>= 3;
            }
            color = Palette[bright + (attribute & 0x07)];
        }

        void operator= (const sColor& other)
        {
            color = other;
        }

        sColor color;
    };

    struct ZXProperty
    {
        uint32_t size;
        uint32_t w;
        uint32_t h;
        uint32_t blockHeight;
        uint32_t dx;
        uint32_t dy;

        enum class Type : uint32_t
        {
            Scr, // done
            ScS, // done
            Bsc, // done
            Atr, // done
            Mc1, // done
            Mc2, // done
            Mc4, // done
            BMc4, // done
            Img,
            Mg1,
            Mg2,
            Mg4,
            Mg8,
            Mgs,
            // Chr,

            Unknown
        };

        Type type;

        const char* formatName;
    };

    ZXProperty getType(uint32_t size)
    {
        const ZXProperty sizesList[] =
        {
            { 6912, 256, 192, 8, 0, 0, ZXProperty::Type::Scr, "zx-scr" },
            { 6929, 256, 192, 8, 0, 0, ZXProperty::Type::ScS, "zx-scr$" },
            { 11136, 384, 304, 8, 64, 64, ZXProperty::Type::Bsc, "zx-bsc" },
            { 768, 256, 192, 8, 0, 0, ZXProperty::Type::Atr, "zx-atr" },
            { 12288, 256, 192, 1, 0, 0, ZXProperty::Type::Mc1, "zx-mc1" },
            { 9216, 256, 192, 2, 0, 0, ZXProperty::Type::Mc2, "zx-mc2" },
            { 7680, 256, 192, 4, 0, 0, ZXProperty::Type::Mc4, "zx-mc4" },
            { 11904, 384, 304, 4, 64, 64, ZXProperty::Type::BMc4, "zx-bmc4" },
            { 13824, 256, 192, 8, 0, 0, ZXProperty::Type::Img, "zx-img" },
            { 19456, 320, 240, 1, 32, 24, ZXProperty::Type::Mg1, "zx-mg1" },
            { 18688, 320, 240, 2, 32, 24, ZXProperty::Type::Mg2, "zx-mg2" },
            { 15616, 320, 240, 4, 32, 24, ZXProperty::Type::Mg4, "zx-mg4" },
            { 14080, 320, 240, 8, 32, 24, ZXProperty::Type::Mg8, "zx-mg8" },
            { 36871, 320, 240, 0, 32, 24, ZXProperty::Type::Mgs, "zx-mgs" },
            // { 41479, 256, 192, 8, 0, 0, ZXProperty::Type::Chr, "zx-chr$" },
        };

        for (size_t i = 0; i < helpers::countof(sizesList); i++)
        {
            const auto& s = sizesList[i];
            if (s.size == size)
            {
                return s;
            }
        }

        return { 0, 0, 0, 0, 0, 0, ZXProperty::Type::Unknown, "" };
    }

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

    void makeBorder(sBitmapDescription& desc, uint8_t zxColor)
    {
        auto pixel = (sPixelRGB*)desc.bitmap.data();
        const auto color = Palette[zxColor & 0x07];

        for (uint32_t i = 0, size = desc.width * desc.height; i < size; i++)
        {
            pixel[i] = color;
        }
    }

    void makeBorder(sBitmapDescription& desc, const uint8_t* zxBorder)
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

    void fillThird(uint32_t layer, const uint8_t* zxPixels, const uint8_t* zxColors,
                   sBitmapDescription& desc, uint32_t blockHeight, sPixelRGB* out)
    {
        zxPixels += 2048 * layer;
        zxColors += 2048 / blockHeight * layer;

        for (uint32_t y = 0; y < 64; y++)
        {
            const uint32_t line = (y * 8) % 64 + (y * 8) / 64;
            auto startLine = &out[line * desc.width];
            for (uint32_t x = 0; x < 256 / 8; x++)
            {
                const uint8_t pixels = *zxPixels++;
                const uint8_t attribute = zxColors[(line / blockHeight) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
            }
        }
    }

    void fillLinear(const uint8_t* zxPixels, const uint8_t* zxColors, uint32_t blockHeight, uint32_t outWidth, sPixelRGB* out)
    {
        for (uint32_t y = 0; y < 192; y++)
        {
            auto startLine = &out[y * outWidth];
            for (uint32_t x = 0; x < 256 / 8; x++)
            {
                const uint8_t pixels = *zxPixels++;
                const uint8_t attribute = zxColors[(y / blockHeight) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
            }
        }
    }

    void loadScr(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        if (prop.type == ZXProperty::Type::ScS)
        {
            desc.exifList.push_back({ "Comment", (const char*)buffer });
            buffer += 17;
        }
        const uint8_t* zxPixels = buffer;
        const uint8_t* zxColors = buffer + 6144;

        for (uint32_t i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + ((prop.dy + 64 * i) * desc.pitch)) + prop.dx;
            fillThird(i, zxPixels, zxColors, desc, 8, out);
        }
    }

    void loadBsc(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        const uint8_t* zxPixels = buffer;
        const uint8_t* zxColors = buffer + 6144;

        for (uint32_t i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + ((prop.dy + 64 * i) * desc.pitch)) + prop.dx;
            fillThird(i, zxPixels, zxColors, desc, 8, out);
        }

        const uint8_t* zxBorder = buffer + 6144 + 768;
        makeBorder(desc, zxBorder);
    }

    void loadAtr(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        const uint8_t px[2] = { 0x55, 0xaa }; // { 0b01010101, 0b10101010 };

        const uint8_t* zxColors = buffer;
        auto out = (sPixelRGB*)(desc.bitmap.data() + prop.dy * desc.pitch) + prop.dx;

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

    void loadMcX(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        const uint32_t blockHeight = prop.blockHeight;

        const uint8_t* zxPixels = buffer;
        const uint8_t* zxColors = buffer + 6144;

        if (prop.type == ZXProperty::Type::Mc1)
        {
            auto out = (sPixelRGB*)desc.bitmap.data();
            fillLinear(zxPixels, zxColors, blockHeight, desc.width, out);
        }
        else
        {
            for (uint32_t i = 0; i < 3; i++)
            {
                auto out = (sPixelRGB*)(desc.bitmap.data() + desc.pitch * 64 * i);
                fillThird(i, zxPixels, zxColors, desc, blockHeight, out);
            }
        }
    }

    void loadBMc4(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        if (prop.type == ZXProperty::Type::BMc4)
        {
            for (uint32_t i = 0; i < 3; i++)
            {
                auto out = (sPixelRGB*)(desc.bitmap.data() + ((prop.dy + 64 * i) * desc.pitch)) + prop.dx;
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
            makeBorder(desc, zxBorder);
        }
    }

    void loadImg(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        const uint8_t* zxPixels = buffer;
        const uint8_t* zxColors = buffer + 6144;

        for (uint32_t i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + ((prop.dy + 64 * i) * desc.pitch)) + prop.dx;
            fillThird(i, zxPixels, zxColors, desc, prop.blockHeight, out);
        }
    }

    bool loadMgX(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        if (buffer[0] != 'M' || buffer[1] != 'G' || buffer[2] != 'H')
        {
            return false;
        }

        const auto zxBorder = buffer[5]; // buffer[6];
        makeBorder(desc, zxBorder);

        buffer += 256; // skip header

        const uint8_t* zxPixels = buffer;
        const uint8_t* zxColors = buffer + 6144 * 2;

        for (uint32_t i = 0; i < 3; i++)
        {
            auto out = (sPixelRGB*)(desc.bitmap.data() + ((prop.dy + 64 * i) * desc.pitch)) + prop.dx;
            fillThird(i, zxPixels, zxColors, desc, prop.blockHeight, out);
        }

        return true;
    }

    bool loadMgs(const uint8_t* buffer, sBitmapDescription& desc, const ZXProperty& prop)
    {
        if (buffer[0] != 'M' || buffer[1] != 'G' || buffer[2] != 'S')
        {
            return false;
        }

        const uint32_t blockHeight = buffer[4];

        const auto zxBorder = buffer[5]; // buffer[6];
        makeBorder(desc, zxBorder);

        buffer += 7; // skip header

        const uint8_t* zxPixels = buffer;
        const uint8_t* zxColors = buffer + 6144 * 2;

        auto out = (sPixelRGB*)(desc.bitmap.data() + prop.dy * desc.pitch) + prop.dx;

        for (uint32_t y = 0; y < 192; y++)
        {
            auto startLine = &out[y * desc.width];
            auto colors = &zxColors[(y / blockHeight) * 64];
            for (uint32_t x = 0; x < 256 / 8; x++)
            {
                const uint8_t pixels = *zxPixels++;
                const uint8_t ink = colors[x * 2 + 0];
                const uint8_t paper = colors[x * 2 + 1];
                const uint8_t attribute = ink | (paper << 3);//zxColors[(y / blockHeight) * 32 + x];
                putEightPixels(&startLine[x * 8], pixels, attribute);
            }
        }

        return true;
    }
}

bool cFormatScr::isSupported(cFile& file, Buffer& /*buffer*/) const
{
    const auto screen = getType(file.getSize());
    return screen.type != ZXProperty::Type::Unknown;
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
    if (screen.type == ZXProperty::Type::Unknown)
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

    bool result = true;

    switch (screen.type)
    {
    case ZXProperty::Type::Scr:
    case ZXProperty::Type::ScS:
        loadScr(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::Bsc:
        loadBsc(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::Atr:
        loadAtr(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::Mc1:
    case ZXProperty::Type::Mc2:
    case ZXProperty::Type::Mc4:
        loadMcX(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::BMc4:
        loadBMc4(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::Img:
        loadImg(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::Mg1:
    case ZXProperty::Type::Mg2:
    case ZXProperty::Type::Mg4:
    case ZXProperty::Type::Mg8:
        result = loadMgX(buffer.data(), desc, screen);
        break;

    case ZXProperty::Type::Mgs:
        result = loadMgs(buffer.data(), desc, screen);
        if (result)
        {
            const uint32_t blockHeight = buffer[4];
            static char formatName[20];
            ::snprintf(formatName, sizeof(formatName), "zx-mgs%u", blockHeight);
            m_formatName = formatName;
        }
        break;

    case ZXProperty::Type::Unknown: // prevent compiler warning
        break;
    }

    return result;
}
