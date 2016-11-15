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

#include <string>

struct GifColorType;

class CFormatGif final : public CFormat
{
public:
    CFormatGif(const char* lib, const char* name, iCallbacks* callbacks);
    ~CFormatGif();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
    bool LoadSubImageImpl(unsigned subImage, sBitmapDescription& desc) override;

    bool load(unsigned subImage, sBitmapDescription& desc);
    void putPixel(sBitmapDescription& desc, int pos, const GifColorType* color, bool transparent);

private:
    std::string m_filename;
};
