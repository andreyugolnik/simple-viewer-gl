/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#if defined(OPENEXR_SUPPORT)

#include "format.h"

#include <string>

class cFormatExr final : public cFormat
{
public:
    cFormatExr(const char* lib, iCallbacks* callbacks);
    ~cFormatExr();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
    bool LoadSubImageImpl(unsigned current, sBitmapDescription& desc) override;

private:
    bool load(unsigned current, sBitmapDescription& desc);

private:
    std::string m_filename;
};

#endif
