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

class cFormatPnm final : public cFormat
{
public:
    cFormatPnm(const char* lib, iCallbacks* callbacks);
    ~cFormatPnm();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
