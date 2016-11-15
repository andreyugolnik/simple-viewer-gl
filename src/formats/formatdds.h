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
    ~CFormatDds();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
