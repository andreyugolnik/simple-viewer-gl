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

class CFormatPsd final : public CFormat
{
public:
    CFormatPsd(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatPsd();

    virtual bool Load(const char* filename, sBitmapDescription& desc) override;
};
