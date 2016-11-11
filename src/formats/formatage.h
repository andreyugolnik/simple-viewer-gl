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

class cFormatAge final : public CFormat
{
public:
    cFormatAge(const char* lib, const char* name, iCallbacks* callbacks);
    ~cFormatAge();

    bool Load(const char* filename, sBitmapDescription& desc) override;
    bool isSupported(cFile& file, Buffer& buffer) const override;
};
