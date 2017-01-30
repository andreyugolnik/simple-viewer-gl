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

class cFormatPpm final : public cFormat
{
public:
    cFormatPpm(const char* lib, iCallbacks* callbacks);
    ~cFormatPpm();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
