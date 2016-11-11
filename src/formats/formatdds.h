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

class CFormatDds final : public CFormat
{
public:
    CFormatDds(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatDds();

    virtual bool Load(const char* filename, sBitmapDescription& desc) override;
};
