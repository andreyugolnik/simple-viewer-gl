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

class cFormatPsd final : public cFormat
{
public:
    cFormatPsd(const char* lib, iCallbacks* callbacks);
    ~cFormatPsd();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
