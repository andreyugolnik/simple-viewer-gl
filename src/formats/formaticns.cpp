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

        cFormatIcns::Entry::Compression compression;

        uint32_t srcBpp;
        uint32_t dstBpp;
        uint32_t iconSize;
    };

    const ChunkDescription TypesList[] = {
        { "ICON", cFormatIcns::Entry::Compression::None, 1, 8, 32 },     // 128	32	1.0	32×32 1-bit mono icon
        { "ICN#", cFormatIcns::Entry::Compression::None, 1, 8, 32 },     // 256	32	6.0	32×32 1-bit mono icon with 1-bit mask
        { "icm#", cFormatIcns::Entry::Compression::None, 1, 8, 16 },     // 48	16	6.0	16×12 1 bit mono icon with 1-bit mask
        { "icm4", cFormatIcns::Entry::Compression::None, 4, 8, 16 },     // 96	16	7.0	16×12 4 bit icon
        { "icm8", cFormatIcns::Entry::Compression::None, 8, 8, 16 },     // 192	16	7.0	16×12 8 bit icon
        { "ics#", cFormatIcns::Entry::Compression::None, 1, 8, 16 },     // 64 (32 img + 32 mask)	16	6.0	16×16 1-bit mask
        { "ics4", cFormatIcns::Entry::Compression::None, 4, 8, 16 },     // 128	16	7.0	16×16 4-bit icon
        { "ics8", cFormatIcns::Entry::Compression::None, 8, 8, 16 },     // 256	16	7.0	16x16 8 bit icon
        { "is32", cFormatIcns::Entry::Compression::Pack, 32, 32, 16 },   // varies (768)	16	8.5	16×16 24-bit icon
        { "s8mk", cFormatIcns::Entry::Compression::None, 8, 8, 16 },     // 256	16	8.5	16x16 8-bit mask
        { "icl4", cFormatIcns::Entry::Compression::None, 4, 8, 32 },     // 512	32	7.0	32×32 4-bit icon
        { "icl8", cFormatIcns::Entry::Compression::None, 8, 8, 32 },     // 1,024	32	7.0	32×32 8-bit icon
        { "il32", cFormatIcns::Entry::Compression::Pack, 32, 32, 32 },   // varies (3,072)	32	8.5	32x32 24-bit icon
        { "l8mk", cFormatIcns::Entry::Compression::None, 8, 8, 32 },     // 1,024	32	8.5	32×32 8-bit mask
        { "ich#", cFormatIcns::Entry::Compression::None, 1, 8, 48 },     // 288	48	8.5	48×48 1-bit mask
        { "ich4", cFormatIcns::Entry::Compression::None, 4, 8, 48 },     // 1,152	48	8.5	48×48 4-bit icon
        { "ich8", cFormatIcns::Entry::Compression::None, 8, 8, 48 },     // 2,304	48	8.5	48×48 8-bit icon
        { "ih32", cFormatIcns::Entry::Compression::Pack, 24, 24, 48 },   // varies (6,912)	48	8.5	48×48 24-bit icon
        { "h8mk", cFormatIcns::Entry::Compression::None, 8, 8, 48 },     // 2,304	48	8.5	48×48 8-bit mask
        { "it32", cFormatIcns::Entry::Compression::Pack, 32, 32, 128 },  // varies (49,152)	128	10.0	128×128 24-bit icon
        { "t8mk", cFormatIcns::Entry::Compression::None, 8, 8, 128 },    // 16,384	128	10.0	128×128 8-bit mask
        { "icp4", cFormatIcns::Entry::Compression::PngJ, 32, 32, 16 },   // varies	16	10.7	16x16 icon in JPEG 2000 or PNG format
        { "icp5", cFormatIcns::Entry::Compression::PngJ, 32, 32, 32 },   // varies	32	10.7	32x32 icon in JPEG 2000 or PNG format
        { "icp6", cFormatIcns::Entry::Compression::PngJ, 32, 32, 64 },   // varies	64	10.7	64x64 icon in JPEG 2000 or PNG format
        { "ic07", cFormatIcns::Entry::Compression::PngJ, 32, 32, 128 },  // varies	128	10.7	128x128 icon in JPEG 2000 or PNG format
        { "ic08", cFormatIcns::Entry::Compression::PngJ, 32, 32, 256 },  // varies	256	10.5	256×256 icon in JPEG 2000 or PNG format
        { "ic09", cFormatIcns::Entry::Compression::PngJ, 32, 32, 512 },  // varies	512	10.5	512×512 icon in JPEG 2000 or PNG format
        { "ic10", cFormatIcns::Entry::Compression::PngJ, 32, 32, 1024 }, // varies	1024	10.7	1024×1024 in 10.7 (or 512x512@2x "retina" in 10.8) icon in JPEG 2000 or PNG format
        { "ic11", cFormatIcns::Entry::Compression::PngJ, 32, 32, 32 },   // varies	32	10.8	16x16@2x "retina" icon in JPEG 2000 or PNG format
        { "ic12", cFormatIcns::Entry::Compression::PngJ, 32, 32, 64 },   // varies	64	10.8	32x32@2x "retina" icon in JPEG 2000 or PNG format
        { "ic13", cFormatIcns::Entry::Compression::PngJ, 32, 32, 256 },  // varies	256	10.8	128x128@2x "retina" icon in JPEG 2000 or PNG format
        { "ic14", cFormatIcns::Entry::Compression::PngJ, 32, 32, 512 },  // varies	512	10.8	256x256@2x "retina" icon in JPEG 2000 or PNG format
    };

    uint32_t ChunkTypeToIndex(const uint8_t* chunk)
    {
        for (uint32_t i = 0; i < sizeof(TypesList) / sizeof(TypesList[0]); i++)
        {
            auto& e = TypesList[i];

            if (::memcmp(e.id, chunk, 4) == 0)
            {
// #if defined(DEBUG)
                ::printf("Type: %c%c%c%c\n", e.id[0], e.id[1], e.id[2], e.id[3]);
// #endif

                return i;
            }
        }

        ::printf("(EE) Unexpected type %c%c%c%c\n", chunk[0], chunk[1], chunk[2], chunk[3]);

        return (uint32_t)-1;
    }

    const ChunkDescription* getDescription(const IcnsChunk& chunk)
    {
        auto idx = ChunkTypeToIndex(chunk.type);
        if (idx != (uint32_t)-1)
        {
            const auto dataLen = helpers::read_uint32(chunk.dataLen);
            auto& e = TypesList[idx];

// #if defined(DEBUG)
            ::printf("   Data Length: %u\n", dataLen);
            ::printf("   Icon Size: %ux%u\n", e.iconSize, e.iconSize);
            ::printf("   Icon Bpp: %u (%u)\n", e.srcBpp, e.dstBpp);
            ::printf("   %s\n", e.compression ? "Compressed" : "Plain data");
// #endif

            return &e;
        }

        return nullptr;
    }

    void UnpackBits(uint8_t* buffer, const uint8_t* chunk, uint32_t size)
    {
        for (uint32_t i = 0; i < size; i++)
        {
            uint8_t hex = chunk[i];

            if (hex >= 128)
            {
                uint32_t count = 256 - hex;

                i++;
                auto value = chunk[i];
                for (uint32_t j = 0; j <= count; j++)
                {
                    *buffer++ = value;
                }

            }
            else
            {
                uint32_t count = hex;
                for (uint32_t j = 0; j <= count; j++)
                {
                    *buffer++ = chunk[i + j + 1];
                }

                i += count;
            }
        }
    }

    struct RGBA
    {
        uint8_t r, g, b, a;
    };

    struct ICNSA
    {
        uint8_t g, r, b, a;
    };

    void ICNSAtoRGBA(uint8_t* buffer, const uint8_t* chunk, uint32_t size)
    {
        auto src = (const ICNSA*)chunk;
        auto dst = (RGBA*)buffer;
        for (uint32_t i = 0; i < size / 4; i++)
        {
            dst[i].r = src[i].r;
            dst[i].g = src[i].g;
            dst[i].b = src[i].b;
            dst[i].a = src[i].a;
        }
    }

    struct RGB
    {
        uint8_t r, g, b;
    };

    struct ICNS
    {
        uint8_t g, r, b;
    };

    void ICNStoRGB(uint8_t* buffer, const uint8_t* chunk, uint32_t size)
    {
        auto src = (const ICNS*)chunk;
        auto dst = (RGBA*)buffer;
        for (uint32_t i = 0; i < size / 4; i++)
        {
            dst[i].r = src[i].r;
            dst[i].g = src[i].g;
            dst[i].b = src[i].b;
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

    uint32_t offset = sizeof(IcnsHeader);

    auto toc = reinterpret_cast<const IcnsChunk*>(icon + offset);
    if (::memcmp("TOC ", toc->type, 4) == 0)
    {
        const uint32_t dataLen = helpers::read_uint32(toc->dataLen);
        const uint32_t count = dataLen / sizeof(IcnsHeader);
        ::printf("TOC len: %u, count: %u\n", dataLen, count);

        offset += dataLen;
        toc++;

        for (uint32_t i = 0; i < count; i++)
        {
            const auto chunkSize = helpers::read_uint32(toc[i].dataLen);
            auto chunk = getDescription(toc[i]);

            if (chunk != nullptr)
            {
                const uint32_t size = chunkSize - sizeof(IcnsChunk);
                ::printf("   offset %u\n", offset);
                ::printf("   chunk size %u\n", chunkSize);
                ::printf("   size %u\n", size);
                m_entries.push_back({ chunk->compression, offset, size, chunk->iconSize, chunk->srcBpp, chunk->dstBpp });
            }

            offset += chunk->iconSize;
        }
    }
    else
    {
        // for (uint32_t i = offset; i < size;)
        // {
            // auto& chunk = getDescription(icon + i);
            // if (chunk.id[0] != 0)
            // {
                // m_entries.push_back(i);
            // }

            // i += chunk.chunkSize;
        // }
    }

    desc.images = (uint32_t)m_entries.size();

    return desc.images > 0 && load(0, desc);
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

    const auto& entry = m_entries[current];
    auto data = m_icon.data() + entry.offset;

    desc.format = entry.dstBpp == 32 ? GL_RGBA : (entry.dstBpp == 24 ? GL_RGB : GL_LUMINANCE);
    desc.bpp = entry.dstBpp;
    desc.pitch = entry.dstBpp * entry.iconSize / 8;
    desc.width = entry.iconSize;
    desc.height = entry.iconSize;

    desc.bppImage = entry.srcBpp;

    m_formatName = "icns";

    desc.bitmap.resize(desc.pitch * desc.height);
    auto buffer = desc.bitmap.data();

    if (entry.compression == Entry::Compression::PngJ)
    {
        m_formatName = "icns/png";

        cPngReader reader;
        reader.setProgressCallback([this](float percent) {
            updateProgress(percent);
        });

        // auto data = icon + sizeof(IcnsChunk);
        // auto size = chunk.chunkSize - sizeof(IcnsChunk);
        if (reader.loadPng(desc, data, entry.size))
        {
            auto& iccProfile = reader.getIccProfile();
            if (iccProfile.size() != 0)
            {
                if (applyIccProfile(desc, iccProfile.data(), iccProfile.size()))
                {
                    m_formatName = "icns/png/icc";
                }
            }
        }
        else
        {
            ::printf("(EE) Error loading PNG frame.\n");
        }
    }
    else
    {
        if (entry.compression == Entry::Compression::Pack)
        {
            UnpackBits(buffer, data, entry.size);
        }

        if (entry.srcBpp == 32)
        {
            ICNSAtoRGBA(buffer, data, entry.size);
        }
        else if (entry.srcBpp == 24)
        {
            ICNStoRGB(buffer, data, entry.size);
        }
        else
        {
            ::memcpy(buffer, data, entry.size);
        }
    }

    return true;
}
