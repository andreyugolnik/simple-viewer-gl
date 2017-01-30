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

class cFormatJpeg final : public cFormat
{
public:
    cFormatJpeg(const char* lib, iCallbacks* callbacks);
    ~cFormatJpeg();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
