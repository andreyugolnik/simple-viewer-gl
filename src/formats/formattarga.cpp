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

#if 0
    void invert(sBitmapDescription& desc)
    {
        auto tga_data = desc.bitmap.data();
        int tga_comp = desc.bpp / 8;

        for (uint32_t j = 0; j * 2 < desc.height; ++j)
        {
            uint32_t index1 = j * desc.width * tga_comp;
            uint32_t index2 = (desc.height - 1 - j) * desc.width * tga_comp;
            for (uint32_t i = desc.width * tga_comp; i > 0; --i)
            {
                const uint8_t temp = tga_data[index1];
                tga_data[index1] = tga_data[index2];
                tga_data[index2] = temp;
                ++index1;
                ++index2;
            }
        }
    }
#endif

    bool colormapped(const sTARGAHeader& header, const uint8_t* tga, sBitmapDescription& desc)
    {
        // Uncompressed Color-mapped Image
        if (header.imageType == 1)
        {
            if (header.pixelDepth == 8)
            {
                if (header.colorMapEntrySize == 24)
                {
                    desc.bppImage = 8;
                    desc.bpp = 24;
                    desc.pitch = desc.width * 3;
                    desc.bitmap.resize(desc.pitch * desc.height);
                    auto out = desc.bitmap.data();

                    uint32_t tgaPitch = header.width;
                    auto cmdData = tga + header.idLength;
                    uint32_t cmtWidth = header.colorMapEntrySize / 8;
                    tga += header.colorMapLength * cmtWidth;

                    for (uint32_t y = 0; y < header.height; y++)
                    {
                        uint32_t dp = (header.height - y - 1) * desc.pitch;
                        uint32_t sp = (header.height - y - 1) * tgaPitch;
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
                else
                {
                    ::printf("(EE) uncompressed 8 bit with non 24 bit color map entry size currently not supported\n");
                }
            }
            else
            {
                ::printf("(EE) non 8 bit color-mapped format\n");
            }

            return false;
        }
        // Compressed Color-mapped Image
        else if (header.imageType == 9)
        {
            ::printf("(EE) compressed 8 bit currently not supported\n");
            return false;
        }
        else
        {
            ::printf("(EE) unknown color-mapped format\n");
            return false;
        }
    }

    bool rgbUncompressed(const sTARGAHeader& header, const uint8_t* tga, sBitmapDescription& desc)
    {
        if (header.pixelDepth == 16)
        {
            desc.bppImage = 16;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            uint32_t tgaPitch = header.width * 2;

            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t dp = (header.height - y - 1) * desc.pitch;
                uint32_t sp = (header.height - y - 1) * tgaPitch;
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
            uint32_t pitch = header.width * 3;

            desc.bppImage = 24;
            desc.bpp = 24;
            desc.pitch = pitch;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t idx = (header.height - y - 1) * pitch;
                for (uint32_t x = 0; x < header.width; x++)
                {
                    out[idx + 0] = tga[idx + 2];
                    out[idx + 1] = tga[idx + 1];
                    out[idx + 2] = tga[idx + 0];
                    idx += 3;
                }
            }
        }
        else if (header.pixelDepth == 32)
        {
            uint32_t pitch = header.width * 4;

            desc.bpp = 32;
            desc.bppImage = 32;
            desc.pitch = pitch;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            for (uint32_t y = 0; y < header.height; y++)
            {
                uint32_t idx = (header.height - y - 1) * pitch;
                for (uint32_t x = 0; x < header.width; x++)
                {
                    out[idx + 0] = tga[idx + 2];
                    out[idx + 1] = tga[idx + 1];
                    out[idx + 2] = tga[idx + 0];
                    out[idx + 3] = tga[idx + 3];
                    idx += 4;
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
        uint32_t width  = 0;
        uint32_t height = 0;
        uint32_t dp     = 0;
        uint32_t sp     = 0;

        if (header.pixelDepth == 16)
        {
            desc.bppImage = 16;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);
            auto out = desc.bitmap.data();

            while (height < header.height)
            {
                uint8_t cunkHead = tga[sp++];
                uint8_t isPacked = cunkHead & 128;
                uint8_t count    = (cunkHead & 127) + 1;

                if (isPacked == 0)
                {
                    for (uint32_t x = 0; x < count; x++)
                    {
                        if (width == header.width)
                        {
                            width = 0;
                            height++;
                            if (height == header.height)
                            {
                                break;
                            }
                        }
                        auto c = *(uint16_t*)&tga[sp];
                        out[dp + 0] = (uint8_t)(((c >>  0) & 31) * 255) / 31;
                        out[dp + 1] = (uint8_t)(((c >>  5) & 31) * 255) / 31;
                        out[dp + 2] = (uint8_t)(((c >> 10) & 31) * 255) / 31;
                        dp += 3;
                        sp += 2;
                        width++;
                    }
                }
                else
                {
                    auto c = *(uint16_t*)&tga[sp];
                    auto r = (uint8_t)(((c >>  0) & 31) * 255) / 31;
                    auto g = (uint8_t)(((c >>  5) & 31) * 255) / 31;
                    auto b = (uint8_t)(((c >> 10) & 31) * 255) / 31;
                    sp += 2;

                    for (uint32_t x = 0; x < count; x++)
                    {
                        if (width == header.width)
                        {
                            width = 0;
                            height++;
                            if (height == header.height)
                            {
                                break;
                            }
                        }
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        dp += 3;
                        width++;
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

            while (height < header.height)
            {
                uint8_t cunkHead = tga[sp++];
                uint8_t isPacked = cunkHead & 128;
                uint8_t count    = (cunkHead & 127) + 1;

                if (isPacked == 0)
                {
                    for (uint32_t x = 0; x < count; x++)
                    {
                        if (width == header.width)
                        {
                            width = 0;
                            height++;
                            if (height == header.height)
                            {
                                break;
                            }
                        }
                        out[dp + 0] = tga[sp + 2];
                        out[dp + 1] = tga[sp + 1];
                        out[dp + 2] = tga[sp + 0];
                        dp += 3;
                        sp += 3;
                        width++;
                    }
                }
                else
                {
                    uint8_t r = tga[sp + 2];
                    uint8_t g = tga[sp + 1];
                    uint8_t b = tga[sp + 0];
                    sp += 3;
                    for (uint32_t x = 0; x < count; x++)
                    {
                        if (width == header.width)
                        {
                            width = 0;
                            height++;
                            if (height == header.height)
                            {
                                break;
                            }
                        }
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        dp += 3;
                        width++;
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

            while (height < header.height)
            {
                uint8_t cunkHead = tga[sp++];
                uint8_t isPacked = cunkHead & 128;
                uint8_t count    = (cunkHead & 127) + 1;

                if (isPacked == 0)
                {
                    for (uint32_t x = 0; x < count; x++)
                    {
                        if (width == header.width)
                        {
                            width = 0;
                            height++;
                            // dp += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (height == header.height)
                            {
                                break;
                            }
                        }
                        uint32_t dp = desc.pitch * (desc.height - height - 1) + width * 4;
                        out[dp + 0] = tga[sp + 2];
                        out[dp + 1] = tga[sp + 1];
                        out[dp + 2] = tga[sp + 0];
                        out[dp + 3] = tga[sp + 3];
                        dp += 4;
                        sp += 4;
                        width++;
                    }
                }
                else
                {
                    uint8_t r = tga[sp + 2];
                    uint8_t g = tga[sp + 1];
                    uint8_t b = tga[sp + 0];
                    uint8_t a = tga[sp + 3];
                    sp += 4;

                    for (uint32_t x = 0; x < count; x++)
                    {
                        if (width == header.width)
                        {
                            width = 0;
                            height++;
                            // dp  += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (height == header.height)
                            {
                                break;
                            }
                        }
                        uint32_t dp = desc.pitch * (desc.height - height - 1) + width * 4;
                        out[dp + 0] = r;
                        out[dp + 1] = g;
                        out[dp + 2] = b;
                        out[dp + 3] = a;
                        dp += 4;
                        width++;
                    }
                }
            }
        }
        else
        {
            ::printf("(EE) unsupported compressed RGB format\n");
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
           && (h->imageType <= 3 || (h->imageType >= 9 && h->imageType <= 11))
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

    // ::printf("------------------------\n");
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

    if (header.colorMapType == 1)
    {
        colormapped(header, tga_data, desc);
        m_formatName = "targa/palette";
    }
    else if (header.colorMapType == 0)
    {
        // RGB - uncompressed
        if (header.imageType == 2)
        {
            rgbUncompressed(header, tga_data, desc);
        }
        // RGB - compressed
        else if (header.imageType == 10)
        {
            rgbCompressed(header, tga_data, desc);
        }
        else
        {
            ::printf("(EE) unknown image type, may be it black and white\n");
            return false;
        }

        m_formatName = "targa/rgb";
    }
    else
    {
        ::printf("(EE) unknown color map type\n");
        return false;
    }

    desc.format = desc.bpp == 32 ? GL_RGBA : GL_RGB;

    return true;
}
