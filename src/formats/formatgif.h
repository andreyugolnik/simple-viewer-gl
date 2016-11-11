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
    virtual ~CFormatGif();

    virtual bool Load(const char* filename, sBitmapDescription& desc) override;
    virtual bool LoadSubImage(unsigned subImage, sBitmapDescription& desc) override;

private:
    bool load(unsigned subImage, sBitmapDescription& desc);
    void putPixel(sBitmapDescription& desc, int pos, const GifColorType* color, bool transparent);

private:
    std::string m_filename;
};
