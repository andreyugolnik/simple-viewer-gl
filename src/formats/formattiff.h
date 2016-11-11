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

class CFormatTiff final : public CFormat
{
public:
    CFormatTiff(const char* lib, const char* name, iCallbacks* callbacks);
    ~CFormatTiff();

    virtual bool Load(const char* filename, sBitmapDescription& desc) override;
    virtual bool LoadSubImage(unsigned subImage, sBitmapDescription& desc) override;

private:
    bool load(unsigned subImage, sBitmapDescription& desc);

private:
    std::string m_filename;
};
