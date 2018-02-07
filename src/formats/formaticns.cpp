/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formaticns.h"
#include "common/bitmap_description.h"
#include "common/file.h"
#include "common/helpers.h"
#include "formats/PngReader.h"

#include <cstring>

namespace
{
    struct IcnsHeader
    {
        uint8_t magic[4];   // Magic literal, must be "icns" (0x69, 0x63, 0x6e, 0x73)
        uint8_t fileLen[4]; // Length of file, in bytes, msb first
    };

    struct IcnsChunk
    {
        uint8_t type[4];    // Icon type, see OSType below.
        uint8_t dataLen[4]; // Length of data, in bytes (including type and length), msb first
        // Variable Icon data
    };

    struct ChunkDescription
    {
        const char* id;
        uint32_t chunkSize; // copied from header

        uint32_t srcBpp;
        uint32_t dstBpp;
        uint32_t iconSize;
    };

    const ChunkDescription TypesList[] = {
        { "ICON", 0, 1, 8, 32 },     // 128	32	1.0	32×32 1-bit mono icon
        { "ICN#", 0, 1, 8, 32 },     // 256	32	6.0	32×32 1-bit mono icon with 1-bit mask
        { "icm#", 0, 1, 8, 16 },     // 48	16	6.0	16×12 1 bit mono icon with 1-bit mask
        { "icm4", 0, 4, 8, 16 },     // 96	16	7.0	16×12 4 bit icon
        { "icm8", 0, 8, 8, 16 },     // 192	16	7.0	16×12 8 bit icon
        { "ics#", 0, 1, 8, 16 },     // 64 (32 img + 32 mask)	16	6.0	16×16 1-bit mask
        { "ics4", 0, 4, 8, 16 },     // 128	16	7.0	16×16 4-bit icon
        { "ics8", 0, 8, 8, 16 },     // 256	16	7.0	16x16 8 bit icon
        { "is32", 0, 32, 32, 16 },   // varies (768)	16	8.5	16×16 24-bit icon
        { "s8mk", 0, 8, 8, 16 },     // 256	16	8.5	16x16 8-bit mask
        { "icl4", 0, 4, 8, 32 },     // 512	32	7.0	32×32 4-bit icon
        { "icl8", 0, 8, 8, 32 },     // 1,024	32	7.0	32×32 8-bit icon
        { "il32", 0, 32, 32, 32 },   // varies (3,072)	32	8.5	32x32 24-bit icon
        { "l8mk", 0, 8, 8, 32 },     // 1,024	32	8.5	32×32 8-bit mask
        { "ich#", 0, 1, 8, 48 },     // 288	48	8.5	48×48 1-bit mask
        { "ich4", 0, 4, 8, 48 },     // 1,152	48	8.5	48×48 4-bit icon
        { "ich8", 0, 8, 8, 48 },     // 2,304	48	8.5	48×48 8-bit icon
        { "ih32", 0, 24, 24, 48 },   // varies (6,912)	48	8.5	48×48 24-bit icon
        { "h8mk", 0, 8, 8, 48 },     // 2,304	48	8.5	48×48 8-bit mask
        { "it32", 0, 32, 32, 128 },  // varies (49,152)	128	10.0	128×128 24-bit icon
        { "t8mk", 0, 8, 8, 128 },    // 16,384	128	10.0	128×128 8-bit mask
        { "icp4", 0, 32, 32, 16 },   // varies	16	10.7	16x16 icon in JPEG 2000 or PNG format
        { "icp5", 0, 32, 32, 32 },   // varies	32	10.7	32x32 icon in JPEG 2000 or PNG format
        { "icp6", 0, 32, 32, 64 },   // varies	64	10.7	64x64 icon in JPEG 2000 or PNG format
        { "ic07", 0, 32, 32, 128 },  // varies	128	10.7	128x128 icon in JPEG 2000 or PNG format
        { "ic08", 0, 32, 32, 256 },  // varies	256	10.5	256×256 icon in JPEG 2000 or PNG format
        { "ic09", 0, 32, 32, 512 },  // varies	512	10.5	512×512 icon in JPEG 2000 or PNG format
        { "ic10", 0, 32, 32, 1024 }, // varies	1024	10.7	1024×1024 in 10.7 (or 512x512@2x "retina" in 10.8) icon in JPEG 2000 or PNG format
        { "ic11", 0, 32, 32, 32 },   // varies	32	10.8	16x16@2x "retina" icon in JPEG 2000 or PNG format
        { "ic12", 0, 32, 32, 64 },   // varies	64	10.8	32x32@2x "retina" icon in JPEG 2000 or PNG format
        { "ic13", 0, 32, 32, 256 },  // varies	256	10.8	128x128@2x "retina" icon in JPEG 2000 or PNG format
        { "ic14", 0, 32, 32, 512 },  // varies	512	10.8	256x256@2x "retina" icon in JPEG 2000 or PNG format
    };

    const ChunkDescription getDescription(const uint8_t* chunk)
    {
        auto header = reinterpret_cast<const IcnsChunk*>(chunk);
        const auto type = header->type;
        const auto dataLen = helpers::read_uint32(header->dataLen);

        for (auto& e : TypesList)
        {
            if (::memcmp(e.id, type, 4) == 0)
            {
#if defined(DEBUG)
                ::printf("Type: %c%c%c%c\n", e.id[0], e.id[1], e.id[2], e.id[3]);
                ::printf("   Data Length: %u\n", dataLen);
                ::printf("   Icon Size: %ux%u\n", e.iconSize, e.iconSize);
                ::printf("   Icon Bpp: %u (%u)\n", e.srcBpp, e.dstBpp);
#endif

                return { e.id, dataLen, e.srcBpp, e.dstBpp, e.iconSize };
            }
        }

        ::printf("(EE) Unexpected type %c%c%c%c\n", type[0], type[1], type[2], type[3]);

        static const ChunkDescription InvalidId{ "\0\0\0\0", 0, 0, 0, 0 };

        return InvalidId;
    }

    struct RGBA
    {
        uint8_t r, g, b, a;
    };

    struct ICNS
    {
        uint8_t g, r, b, a;
    };

    void ICNStoRGBA(uint8_t* buffer, const uint8_t* chunk, const ChunkDescription& desc)
    {
        const uint32_t size = desc.iconSize * desc.iconSize;
        const uint32_t dataSize = desc.chunkSize - sizeof(IcnsChunk);
        if (size * 4 == dataSize)
        {
            auto src = (const ICNS*)(chunk + sizeof(IcnsChunk));
            auto dst = (RGBA*)(buffer);
            for (uint32_t i = 0; i < size; i++)
            {
                dst[i].r = src[i].r;
                dst[i].g = src[i].g;
                dst[i].b = src[i].b;
                dst[i].a = src[i].a;
            }
        }
        else
        {
            ::printf("(WW) Bitmap size mismatch. Required %u, got %u.\n", size * 4, dataSize);
        }
    }

} // namespace

cFormatIcns::cFormatIcns(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatIcns::~cFormatIcns()
{
}

bool cFormatIcns::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, sizeof(IcnsHeader)))
    {
        return false;
    }

    const auto h = reinterpret_cast<const IcnsHeader*>(buffer.data());
    // ::printf("Magic: %c%c%c%c\n", h->magic[0], h->magic[1], h->magic[2], h->magic[3]);

    const uint32_t fileLen = helpers::read_uint32(h->fileLen);
    // ::printf("Length: %u (%u)\n", fileLen, (uint32_t)file.getSize());

    const uint8_t magic[4] = { 'i', 'c', 'n', 's' };

    return fileLen == file.getSize() && ::memcmp(&h->magic, magic, sizeof(magic)) == 0;
}

bool cFormatIcns::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    const uint32_t size = (uint32_t)file.getSize();

    m_icon.resize(size);
    auto icon = m_icon.data();

    if (size != file.read(icon, size))
    {
        return false;
    }

    m_entries.clear();

    for (uint32_t i = (uint32_t)sizeof(IcnsHeader);;)
    {
        auto& chunk = getDescription(icon + i);
        if (chunk.id[0] == 0)
        {
            break;
        }

        m_entries.push_back(i);

        i += chunk.chunkSize;
        if (i >= size)
        {
            break;
        }
    }

    desc.images = (uint32_t)m_entries.size();

    return load(0, desc);
}

bool cFormatIcns::LoadSubImageImpl(uint32_t current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool cFormatIcns::load(uint32_t current, sBitmapDescription& desc)
{
    current = std::max<uint32_t>(current, 0);
    current = std::min<uint32_t>(current, desc.images - 1);

    desc.current = current;

    auto icon = &m_icon[m_entries[current]];

    auto& chunk = getDescription(icon);

    desc.format = chunk.dstBpp == 32 ? GL_RGBA : (chunk.dstBpp == 24 ? GL_RGB : GL_LUMINANCE);
    desc.bpp = chunk.dstBpp;
    desc.pitch = chunk.dstBpp * chunk.iconSize / 8;
    desc.width = chunk.iconSize;
    desc.height = chunk.iconSize;

    desc.bppImage = chunk.srcBpp;

    m_formatName = "icns";

    desc.bitmap.resize(desc.pitch * desc.height);
    auto buffer = desc.bitmap.data();

    if (chunk.id[0] == 'i' && chunk.id[1] == 'c')
    {
        cPngReader reader(m_cms);
        reader.setProgressCallback([this](float percent) {
            updateProgress(percent);
        });

        auto data = icon + sizeof(IcnsChunk);
        auto size = chunk.chunkSize - sizeof(IcnsChunk);
        if (reader.loadPng(desc, data, size) == false)
        {
            ::printf("(EE) Error loading PNG frame.\n");
        }
    }
    else if (chunk.dstBpp == 32)
    {
        ICNStoRGBA(buffer, icon, chunk);
    }
    else if (chunk.dstBpp == 24)
    {
        ::printf("(WW) 24-bit chunk, skip it.\n");
        ::memset(buffer, 0, desc.bitmap.size());
    }
    else if (chunk.dstBpp == 8)
    {
        ::memcpy(buffer, icon + sizeof(IcnsChunk), chunk.chunkSize - sizeof(IcnsChunk));
    }

    return true;
}
