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

public:
    struct Entry
    {
        enum Compression : uint32_t
        {
            None,
            Pack,
            PngJ
        };

        Compression compression;
        uint32_t offset;
        uint32_t size;
        uint32_t iconSize;
        uint32_t srcBpp;
        uint32_t dstBpp;
    };

private:
    std::vector<uint8_t> m_icon;
    std::vector<Entry> m_entries;
};
