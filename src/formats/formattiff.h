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

class cFormatTiff final : public cFormat
{
public:
    cFormatTiff(const char* lib, iCallbacks* callbacks);
    ~cFormatTiff();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
    bool LoadSubImageImpl(unsigned current, sBitmapDescription& desc) override;

    bool load(unsigned current, sBitmapDescription& desc);

private:
    std::string m_filename;
};
