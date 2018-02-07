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
    cFormatIcns(iCallbacks* callbacks);
    ~cFormatIcns();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    virtual bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
    virtual bool LoadSubImageImpl(unsigned current, sBitmapDescription& desc) override;

private:
    bool load(uint32_t current, sBitmapDescription& desc);

private:
    std::vector<uint8_t> m_icon;
    std::vector<uint32_t> m_entries;
};
