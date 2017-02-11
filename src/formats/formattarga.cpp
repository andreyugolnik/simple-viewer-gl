/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formattarga.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

namespace
{

#pragma pack(push, 1)
    struct sTARGAHeader
    {
        uint8_t idLength;
        uint8_t colorMapType;
        uint8_t imageType;

        uint16_t firstEntryIndex;
        uint16_t colorMapLength;
        uint8_t colorMapEntrySize;

        uint16_t xOrigin;
        uint16_t yOrigin;
        uint16_t width;
        uint16_t height;
        uint8_t pixelDepth;
        uint8_t imageDescriptor;
    };
#pragma pack(pop)

    enum class Origin
    {
#if 0
        LowerRight,
        UpperRight,
#endif
        LowerLeft,
        UpperLeft,
    };

    Origin getOrigin(uint8_t imageDescriptor)
    {
        // return (Origin)((imageDescriptor >> 4) & 0x03);
        return (imageDescriptor & (1 << 5)) ? Origin::UpperLeft : Origin::LowerLeft;
    }

    inline uint32_t getIndexUpperLeft(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t components)
    {
        const uint32_t pitch = width * components;
        return y * pitch + x * components;
    }

    inline uint32_t getIndexLowerLeft(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t components)
    {
        const uint32_t pitch = width * components;
        return (height - y - 1) * pitch + x * components;
    }

#if 0
    inline uint32_t getIndexUpperRight(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t components)
    {
        const uint32_t pitch = width * components;
        return y * pitch + (width - x - 1) * components;
    }

    inline uint32_t getIndexLowerRight(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t components)
    {
        const uint32_t pitch = width * components;
        return (height - y - 1) * pitch + (width - x - 1) * components;
    }
#endif

    inline uint32_t getIndex(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t components, Origin origin)
    {
        const uint32_t idx = origin == Origin::LowerLeft
                             ? getIndexLowerLeft(x, y, width, height, components)
                             : getIndexUpperLeft(x, y, width, height, components);
        return idx;
    }

    bool colormapped(const sTARGAHeader& header, const uint8_t* tga, sBitmapDescription& desc)
    {
        if (header.colorMapType != 1)
        {
            ::printf("(EE) Unknown color-mapped format.\n");
            return false;
        }

        if (header.pixelDepth != 8)
        {
            ::printf("(EE) Non 8 bit color-mapped format.\n");
            return false;
        }

        if (header.colorMapEntrySize != 24)
        {
            ::printf("(EE) 8 bit with non 24 bit color map entry size currently not supported.\n");
        }

        desc.bppImage = 8;
        desc.bpp = 24;
        desc.pitch = desc.width * 3;
        desc.bitmap.resize(desc.pitch * desc.height);
        auto out = desc.bitmap.data();

        auto cmdData = tga + header.idLength;
        const uint32_t cmtWidth = header.colorMapEntrySize / 8;
        tga += header.colorMapLength * cmtWidth;

        const auto origin = getOrigin(header.imageDescriptor);

        if (header.imageType == 1)
        {
            // ::printf("(II) Uncompressed color-mapped image.\n");

            uint32_t sp = 0;
            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t dp = getIndex(0, y, desc.width, desc.height, 3, origin);
                for (uint32_t x = 0; x < header.width; x++)
                {
                    out[dp + 0] = cmdData[tga[sp] * cmtWidth + 2];
                    out[dp + 1] = cmdData[tga[sp] * cmtWidth + 1];
                    out[dp + 2] = cmdData[tga[sp] * cmtWidth + 0];
                    dp += 3;
                    sp++;
                }
            }

            return true;
        }
        else if (header.imageType == 9)
        {
            // ::printf("(II) Compressed color-mapped image.\n");

            uint32_t sp = 0;
            uint32_t x = 0;
            uint32_t y = 0;

            while (y < header.height)
            {
                const uint8_t cunkHead = tga[sp++];
                const bool isPacked = (cunkHead & 128) != 0;
                const uint8_t count = (cunkHead & 127) + 1;

                if (isPacked)
                {
                    const uint8_t b = cmdData[tga[sp] * cmtWidth + 0];
                    const uint8_t g = cmdData[tga[sp] * cmtWidth + 1];
                    const uint8_t r = cmdData[tga[sp] * cmtWidth + 2];
                    sp++;

                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 3, origin);
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        x++;
                    }
                }
                else
                {
                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 3, origin);
                        out[dp + 0] = cmdData[tga[sp] * cmtWidth + 2];
                        out[dp + 1] = cmdData[tga[sp] * cmtWidth + 1];
                        out[dp + 2] = cmdData[tga[sp] * cmtWidth + 0];
                        sp++;
                        x++;
                    }
                }
            }

            return true;
        }

        return false;
    }

    bool rgbUncompressed(const sTARGAHeader& header, const uint8_t* tga, sBitmapDescription& desc)
    {
        if (header.colorMapType != 0)
        {
            ::printf("(EE) Unknown color-mapped format.\n");
            return false;
        }

        const auto origin = getOrigin(header.imageDescriptor);
        uint32_t sp = 0;

        if (header.pixelDepth == 16)
        {
            desc.bppImage = 16;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t dp = getIndex(0, y, desc.width, desc.height, 3, origin);
                for (uint32_t x = 0; x < header.width; x++)
                {
                    auto c = *(uint16_t*)&tga[sp];
                    out[dp + 0] = (((c >>  0) & 31) * 255) / 31;
                    out[dp + 1] = (((c >>  5) & 31) * 255) / 31;
                    out[dp + 2] = (((c >> 10) & 31) * 255) / 31;
                    dp += 3;
                    sp += 2;
                }
            }
        }
        else if (header.pixelDepth == 24)
        {
            desc.bppImage = 24;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t dp = getIndex(0, y, desc.width, desc.height, 3, origin);
                for (uint32_t x = 0; x < header.width; x++)
                {
                    out[dp + 0] = tga[sp + 2];
                    out[dp + 1] = tga[sp + 1];
                    out[dp + 2] = tga[sp + 0];
                    dp += 3;
                    sp += 3;
                }
            }
        }
        else if (header.pixelDepth == 32)
        {
            desc.bpp = 32;
            desc.bppImage = 32;
            desc.pitch = desc.width * 4;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t dp = getIndex(0, y, desc.width, desc.height, 4, origin);
                for (uint32_t x = 0; x < header.width; x++)
                {
                    out[dp + 0] = tga[sp + 2];
                    out[dp + 1] = tga[sp + 1];
                    out[dp + 2] = tga[sp + 0];
                    out[dp + 3] = tga[sp + 3];
                    dp += 4;
                    sp += 4;
                }
            }
        }
        else
        {
            ::printf("(EE) unsupported uncompressed RGB format\n");
            return false;
        }

        return true;
    }

    bool rgbCompressed(const sTARGAHeader& header, const uint8_t* tga, sBitmapDescription& desc)
    {
        if (header.colorMapType != 0)
        {
            ::printf("(EE) Unknown color-mapped format.\n");
            return false;
        }

        const auto origin = getOrigin(header.imageDescriptor);
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t sp = 0;

        if (header.pixelDepth == 16)
        {
            desc.bppImage = 16;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            while (y < header.height)
            {
                const uint8_t cunkHead = tga[sp++];
                const bool isPacked = (cunkHead & 128) != 0;
                const uint8_t count = (cunkHead & 127) + 1;

                if (isPacked == false)
                {
                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        auto c = *(uint16_t*)&tga[sp];
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 3, origin);
                        out[dp + 0] = (uint8_t)(((c >>  0) & 31) * 255) / 31;
                        out[dp + 1] = (uint8_t)(((c >>  5) & 31) * 255) / 31;
                        out[dp + 2] = (uint8_t)(((c >> 10) & 31) * 255) / 31;
                        sp += 2;
                        x++;
                    }
                }
                else
                {
                    const auto c = *(uint16_t*)&tga[sp];
                    const auto r = (uint8_t)(((c >>  0) & 31) * 255) / 31;
                    const auto g = (uint8_t)(((c >>  5) & 31) * 255) / 31;
                    const auto b = (uint8_t)(((c >> 10) & 31) * 255) / 31;
                    sp += 2;

                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 3, origin);
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        x++;
                    }
                }
            }
        }
        else if (header.pixelDepth == 24)
        {
            desc.bppImage = 24;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            while (y < header.height)
            {
                const uint8_t cunkHead = tga[sp++];
                const bool isPacked = (cunkHead & 128) != 0;
                const uint8_t count = (cunkHead & 127) + 1;

                if (isPacked == false)
                {
                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 3, origin);
                        out[dp + 0] = tga[sp + 2];
                        out[dp + 1] = tga[sp + 1];
                        out[dp + 2] = tga[sp + 0];
                        sp += 3;
                        x++;
                    }
                }
                else
                {
                    const uint8_t b = tga[sp + 0];
                    const uint8_t g = tga[sp + 1];
                    const uint8_t r = tga[sp + 2];
                    sp += 3;
                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 3, origin);
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        x++;
                    }
                }
            }
        }
        else if (header.pixelDepth == 32)
        {
            desc.bppImage = 32;
            desc.bpp = 32;
            desc.pitch = desc.width * 4;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            while (y < header.height)
            {
                const uint8_t cunkHead = tga[sp++];
                const bool isPacked = (cunkHead & 128) != 0;
                const uint8_t count = (cunkHead & 127) + 1;

                if (isPacked == false)
                {
                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 4, origin);
                        out[dp + 0] = tga[sp + 2];
                        out[dp + 1] = tga[sp + 1];
                        out[dp + 2] = tga[sp + 0];
                        out[dp + 3] = tga[sp + 3];
                        sp += 4;
                        x++;
                    }
                }
                else
                {
                    const uint8_t b = tga[sp + 0];
                    const uint8_t g = tga[sp + 1];
                    const uint8_t r = tga[sp + 2];
                    const uint8_t a = tga[sp + 3];
                    sp += 4;

                    for (uint32_t i = 0; i < count; i++)
                    {
                        if (x == header.width)
                        {
                            x = 0;
                            y++;
                            if (y == header.height)
                            {
                                break;
                            }
                        }
                        const uint32_t dp = getIndex(x, y, desc.width, desc.height, 4, origin);
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        out[dp + 3] = a;
                        x++;
                    }
                }
            }
        }
        else
        {
            ::printf("(EE) Unsupported compressed RGB format\n");
            return false;
        }

        return true;
    }

}

cFormatTarga::cFormatTarga(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatTarga::~cFormatTarga()
{
}

bool cFormatTarga::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, sizeof(sTARGAHeader)))
    {
        return false;
    }

    const auto h = reinterpret_cast<const sTARGAHeader*>(buffer.data());
    return h->colorMapType <= 1
           && h->width > 0 && h->height > 0
           && (h->imageType <= 3 || (h->imageType >= 9 && h->imageType <= 11) || h->imageType == 32 || h->imageType == 33)
           && (h->pixelDepth == 8 || h->pixelDepth == 16 || h->pixelDepth == 24 || h->pixelDepth == 32);
}

bool cFormatTarga::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    std::vector<uint8_t> tga(desc.size);
    if (desc.size != file.read(tga.data(), desc.size))
    {
        ::printf("(EE) Can't read TARGA data.\n");
        return false;
    }

    auto& header = *reinterpret_cast<const sTARGAHeader*>(tga.data());
    auto tga_data = reinterpret_cast<const uint8_t*>(tga.data() + sizeof(sTARGAHeader));

    desc.width = header.width;
    desc.height = header.height;

    // ::printf("(II) idLength:          %u\n", (uint32_t)header.idLength);
    // ::printf("(II) colorMapType:      %u\n", (uint32_t)header.colorMapType);
    // ::printf("(II) imageType:         %u\n", (uint32_t)header.imageType);
    // ::printf("(II) firstEntryIndex:   %u\n", (uint32_t)header.firstEntryIndex);
    // ::printf("(II) colorMapLength:    %u\n", (uint32_t)header.colorMapLength);
    // ::printf("(II) colorMapEntrySize: %u\n", (uint32_t)header.colorMapEntrySize);
    // ::printf("(II) xOrigin:           %u\n", (uint32_t)header.xOrigin);
    // ::printf("(II) yOrigin:           %u\n", (uint32_t)header.yOrigin);
    // ::printf("(II) width:             %u\n", (uint32_t)header.width);
    // ::printf("(II) height:            %u\n", (uint32_t)header.height);
    // ::printf("(II) pixelDepth:        %u\n", (uint32_t)header.pixelDepth);
    // ::printf("(II) imageDescriptor:   %u\n", (uint32_t)header.imageDescriptor);

    bool result = false;

    // 1 - Uncompressed, color-mapped images.
    // 2 - Uncompressed, RGB images.
    // 3 - Uncompressed, black and white images.
    // 9 - Runlength encoded color-mapped images.
    // 10 - Runlength encoded RGB images.
    // 11 - Compressed, black and white images.
    // 32 - Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
    // 33 - Compressed color-mapped data, using Huffman, Delta, and runlength encoding. 4-pass quadtree-type process.
    if (header.imageType == 1 || header.imageType == 9)
    {
        result = colormapped(header, tga_data, desc);
    }
    else if (header.imageType == 2)
    {
        result = rgbUncompressed(header, tga_data, desc);
    }
    else if (header.imageType == 10)
    {
        result = rgbCompressed(header, tga_data, desc);
    }

    if (result == false)
    {
        ::printf("(EE) Unknown image type.\n");
        return false;
    }

    m_formatName = (header.imageType == 9
                    || header.imageType == 10
                    || header.imageType == 11
                    || header.imageType == 32
                    || header.imageType == 33)
                   ?  "targa/rle" : "targa";

    desc.format = desc.bpp == 32 ? GL_RGBA : GL_RGB;

    return true;
}
