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

namespace icns
{
    const char* CompressionToName(Compression compression)
    {
        static const char* Names[] = {
            "None",
            "Pack",
            "PngJ"
        };

        return Names[(uint32_t)compression];
    }

} // namespace icns

cFormatIcns::cFormatIcns(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatIcns::~cFormatIcns()
{
}

bool cFormatIcns::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, sizeof(icns::Header)))
    {
        return false;
    }

    const auto h = reinterpret_cast<const icns::Header*>(buffer.data());
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

    const auto size = (uint32_t)file.getSize();

    m_icon.resize(size);
    auto icon = m_icon.data();

    if (size != file.read(icon, size))
    {
        return false;
    }

    m_entries.clear();

    iterateContent(icon, sizeof(icns::Header), size);

    desc.images = (uint32_t)m_entries.size();

    return desc.images > 0 && load(0, desc);
}

void cFormatIcns::iterateContent(const uint8_t* icon, uint32_t offset, uint32_t size)
{
    while (offset < size)
    {
        auto& chunk = reinterpret_cast<const icns::Chunk&>(icon[offset]);
        const auto chunkSize = helpers::read_uint32(chunk.dataLen);

        auto& desc = getDescription(chunk);
        if (desc.type != icns::Type::Count)
        {
            if (desc.type == icns::Type::TOC_)
            {
                ::printf("---- TOC ----\n");
                offset += chunkSize;
                iterateContent(icon, offset, size);
            }
            else
            {
                Entry entry = desc;

                entry.offset = offset + sizeof(icns::Chunk);
                entry.size = chunkSize - sizeof(icns::Chunk);

                ::printf("   chunk size: %u\n", chunkSize);

                ::printf("   icon bpp: %u -> %u\n", entry.srcBpp, entry.dstBpp);
                ::printf("   icon resolution: %u x %u\n", entry.iconSize, entry.iconSize);

                auto compression = icns::CompressionToName(entry.compression);
                ::printf("   copression: %s\n", compression);

                ::printf("   offset: %u\n", entry.offset);
                ::printf("   size: %u\n", entry.size);

                m_entries.push_back(entry);

                offset += chunkSize;
            }
        }
        else
        {
            offset += (uint32_t)sizeof(icns::Chunk);
        }
    }
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

    ::printf(" Decoding: %s\n", icns::CompressionToName(entry.compression));

    if (entry.compression == icns::Compression::PngJ)
    {
        m_formatName = "icns/png";

        cPngReader reader;
        reader.setProgressCallback([this](float percent) {
            updateProgress(percent);
        });

        // auto data = icon + sizeof(icns::Chunk);
        // auto size = chunk.chunkSize - sizeof(icns::Chunk);
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
        if (entry.compression == icns::Compression::Pack)
        {
            unpackBits(buffer, data, entry.size);
        }
        else if (entry.srcBpp == 32)
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

void cFormatIcns::unpackBits(uint8_t* buffer, const uint8_t* chunk, uint32_t size) const
{
    uint32_t c = 0;

    const auto end = chunk + size;
    while (chunk < end)
    {
        const uint8_t N = *chunk++;

        if (N < 0x80)
        {
            const uint32_t count = N + 1;
            for (uint32_t i = 0; i < count; i++)
            {
                *buffer++ = *chunk++;
                c++;
            }
        }
        else
        {
            const uint32_t count = N - 0x80 + 3;

            const uint8_t value = *chunk++;
            for (uint32_t i = 0; i < count; i++)
            {
                *buffer++ = value;
                c++;
            }
        }
    }

    ::printf("-- %u\n", c);
}

void cFormatIcns::ICNSAtoRGBA(uint8_t* buffer, const uint8_t* chunk, uint32_t size) const
{
    auto src = (const icns::ICNSA*)chunk;
    auto dst = (icns::RGBA*)buffer;
    for (uint32_t i = 0; i < size / 4; i++)
    {
        dst[i].r = src[i].r;
        dst[i].g = src[i].g;
        dst[i].b = src[i].b;
        dst[i].a = src[i].a;
    }
}

void cFormatIcns::ICNStoRGB(uint8_t* buffer, const uint8_t* chunk, uint32_t size) const
{
    auto src = (const icns::ICNS*)chunk;
    auto dst = (icns::RGBA*)buffer;
    for (uint32_t i = 0; i < size / 4; i++)
    {
        dst[i].r = src[i].r;
        dst[i].g = src[i].g;
        dst[i].b = src[i].b;
    }
}

const cFormatIcns::Entry& cFormatIcns::getDescription(const icns::Chunk& chunk) const
{
    struct Pair
    {
        const char* id;
        Entry entry;
    };

    static const Pair List[] = {
        { "TOC ", { icns::Type::TOC_, icns::Compression::None, 0, 0, 0, 0, 0 } },      // Table Of Content
        { "ICON", { icns::Type::ICON, icns::Compression::None, 1, 8, 32, 0, 0 } },     // 128	32	1.0	32×32 1-bit mono icon
        { "ICN#", { icns::Type::ICN3, icns::Compression::None, 1, 8, 32, 0, 0 } },     // 256	32	6.0	32×32 1-bit mono icon with 1-bit mask
        { "icm#", { icns::Type::icm3, icns::Compression::None, 1, 8, 16, 0, 0 } },     // 48	16	6.0	16×12 1 bit mono icon with 1-bit mask
        { "icm4", { icns::Type::icm4, icns::Compression::None, 4, 8, 16, 0, 0 } },     // 96	16	7.0	16×12 4 bit icon
        { "icm8", { icns::Type::icm8, icns::Compression::None, 8, 8, 16, 0, 0 } },     // 192	16	7.0	16×12 8 bit icon
        { "ics#", { icns::Type::ics3, icns::Compression::None, 1, 8, 16, 0, 0 } },     // 64 (32 img + 32 mask)	16	6.0	16×16 1-bit mask
        { "ics4", { icns::Type::ics4, icns::Compression::None, 4, 8, 16, 0, 0 } },     // 128	16	7.0	16×16 4-bit icon
        { "ics8", { icns::Type::ics8, icns::Compression::None, 8, 8, 16, 0, 0 } },     // 256	16	7.0	16x16 8 bit icon
        { "is32", { icns::Type::is32, icns::Compression::Pack, 24, 24, 16, 0, 0 } },   // varies (768)	16	8.5	16×16 24-bit icon
        { "s8mk", { icns::Type::s8mk, icns::Compression::None, 8, 8, 16, 0, 0 } },     // 256	16	8.5	16x16 8-bit mask
        { "icl4", { icns::Type::icl4, icns::Compression::None, 4, 8, 32, 0, 0 } },     // 512	32	7.0	32×32 4-bit icon
        { "icl8", { icns::Type::icl8, icns::Compression::None, 8, 8, 32, 0, 0 } },     // 1,024	32	7.0	32×32 8-bit icon
        { "il32", { icns::Type::il32, icns::Compression::Pack, 32, 32, 32, 0, 0 } },   // varies (3,072)	32	8.5	32x32 24-bit icon
        { "l8mk", { icns::Type::l8mk, icns::Compression::None, 8, 8, 32, 0, 0 } },     // 1,024	32	8.5	32×32 8-bit mask
        { "ich#", { icns::Type::ich3, icns::Compression::None, 1, 8, 48, 0, 0 } },     // 288	48	8.5	48×48 1-bit mask
        { "ich4", { icns::Type::ich4, icns::Compression::None, 4, 8, 48, 0, 0 } },     // 1,152	48	8.5	48×48 4-bit icon
        { "ich8", { icns::Type::ich8, icns::Compression::None, 8, 8, 48, 0, 0 } },     // 2,304	48	8.5	48×48 8-bit icon
        { "ih32", { icns::Type::ih32, icns::Compression::Pack, 24, 24, 48, 0, 0 } },   // varies (6,912)	48	8.5	48×48 24-bit icon
        { "h8mk", { icns::Type::h8mk, icns::Compression::None, 8, 8, 48, 0, 0 } },     // 2,304	48	8.5	48×48 8-bit mask
        { "it32", { icns::Type::it32, icns::Compression::Pack, 32, 32, 128, 0, 0 } },  // varies (49,152)	128	10.0	128×128 24-bit icon
        { "t8mk", { icns::Type::t8mk, icns::Compression::None, 8, 8, 128, 0, 0 } },    // 16,384	128	10.0	128×128 8-bit mask
        { "icp4", { icns::Type::icp4, icns::Compression::PngJ, 32, 32, 16, 0, 0 } },   // varies	16	10.7	16x16 icon in JPEG 2000 or PNG format
        { "icp5", { icns::Type::icp5, icns::Compression::PngJ, 32, 32, 32, 0, 0 } },   // varies	32	10.7	32x32 icon in JPEG 2000 or PNG format
        { "icp6", { icns::Type::icp6, icns::Compression::PngJ, 32, 32, 64, 0, 0 } },   // varies	64	10.7	64x64 icon in JPEG 2000 or PNG format
        { "ic07", { icns::Type::ic07, icns::Compression::PngJ, 32, 32, 128, 0, 0 } },  // varies	128	10.7	128x128 icon in JPEG 2000 or PNG format
        { "ic08", { icns::Type::ic08, icns::Compression::PngJ, 32, 32, 256, 0, 0 } },  // varies	256	10.5	256×256 icon in JPEG 2000 or PNG format
        { "ic09", { icns::Type::ic09, icns::Compression::PngJ, 32, 32, 512, 0, 0 } },  // varies	512	10.5	512×512 icon in JPEG 2000 or PNG format
        { "ic10", { icns::Type::ic10, icns::Compression::PngJ, 32, 32, 1024, 0, 0 } }, // varies	1024	10.7	1024×1024 in 10.7 (or 512x512@2x "retina" in 10.8) icon in JPEG 2000 or PNG format
        { "ic11", { icns::Type::ic11, icns::Compression::PngJ, 32, 32, 32, 0, 0 } },   // varies	32	10.8	16x16@2x "retina" icon in JPEG 2000 or PNG format
        { "ic12", { icns::Type::ic12, icns::Compression::PngJ, 32, 32, 64, 0, 0 } },   // varies	64	10.8	32x32@2x "retina" icon in JPEG 2000 or PNG format
        { "ic13", { icns::Type::ic13, icns::Compression::PngJ, 32, 32, 256, 0, 0 } },  // varies	256	10.8	128x128@2x "retina" icon in JPEG 2000 or PNG format
        { "ic14", { icns::Type::ic14, icns::Compression::PngJ, 32, 32, 512, 0, 0 } },  // varies	512	10.8	256x256@2x "retina" icon in JPEG 2000 or PNG format

        { "icnV", { icns::Type::icnV, icns::Compression::PngJ, 0, 0, 0, 0, 0 } },  // 4-byte big endian float - equal to the bundle version number of Icon Composer.app that created to icon
        { "name", { icns::Type::name, icns::Compression::PngJ, 0, 0, 0, 0, 0 } },  // Unknown
        { "info", { icns::Type::info, icns::Compression::PngJ, 0, 0, 0, 0, 0 } },  // Info binary plist. Usage unknown
    };

    auto type = chunk.type;

    for (auto& e : List)
    {
        if (::memcmp(e.id, type, 4) == 0)
        {
            ::printf("Type: '%s'\n", e.id);

            return e.entry;
        }
    }

    ::printf("(EE) Unexpected chunk type: '%c%c%c%c'\n", type[0], type[1], type[2], type[3]);

    static const Entry Error{ icns::Type::Count, icns::Compression::Count, 0, 0, 0, 0, 0 };
    return Error;
}
