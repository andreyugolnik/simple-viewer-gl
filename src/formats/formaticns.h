/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "format.h"

namespace icns
{
    enum class Type
    {
        TOC_,
        ICON,
        ICN3,
        icm3,
        icm4,
        icm8,
        ics3,
        ics4,
        ics8,
        is32,
        s8mk,
        icl4,
        icl8,
        il32,
        l8mk,
        ich3,
        ich4,
        ich8,
        ih32,
        h8mk,
        it32,
        t8mk,
        icp4,
        icp5,
        icp6,
        ic07,
        ic08,
        ic09,
        ic10,
        ic11,
        ic12,
        ic13,
        ic14,

        icnV,
        name,
        info,

        Count
    };

    enum class Compression : uint32_t
    {
        None,
        Pack,
        PngJ,

        Count
    };

    struct Header
    {
        uint8_t magic[4];   // Magic literal, must be "icns" (0x69, 0x63, 0x6e, 0x73)
        uint8_t fileLen[4]; // Length of file, in bytes, msb first
    };

    struct Chunk
    {
        uint8_t type[4];    // Icon type, see OSType below.
        uint8_t dataLen[4]; // Length of data, in bytes (including type and length), msb first
        // Variable Icon data
    };

    struct RGBA
    {
        uint8_t r, g, b, a;
    };

    struct ICNSA
    {
        uint8_t g, r, b, a;
    };

    struct RGB
    {
        uint8_t r, g, b;
    };

    struct ICNS
    {
        uint8_t g, r, b;
    };

    const char* CompressionToName(Compression compression);

} // namespace icns

class cFormatIcns final : public cFormat
{
public:
    explicit cFormatIcns(iCallbacks* callbacks);
    ~cFormatIcns();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    virtual bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
    virtual bool LoadSubImageImpl(unsigned current, sBitmapDescription& desc) override;

private:
    bool load(uint32_t current, sBitmapDescription& desc);

    void iterateContent(const uint8_t* icon, uint32_t offset, uint32_t size);
    void unpackBits(uint8_t* buffer, const uint8_t* chunk, uint32_t size) const;
    void ICNSAtoRGBA(uint8_t* buffer, const uint8_t* chunk, uint32_t size) const;
    void ICNStoRGB(uint8_t* buffer, const uint8_t* chunk, uint32_t size) const;

    struct Entry;
    const Entry& getDescription(const icns::Chunk& chunk) const;

private:
    std::vector<uint8_t> m_icon;

    struct Entry
    {
        icns::Type type;

        icns::Compression compression;
        uint32_t srcBpp;
        uint32_t dstBpp;
        uint32_t iconSize;

        uint32_t offset;
        uint32_t size;
    };

    std::vector<Entry> m_entries;
};
