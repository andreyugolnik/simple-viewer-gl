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

class cFormatDds final : public cFormat
{
public:
    cFormatDds(const char* lib, iCallbacks* callbacks);
    ~cFormatDds();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
