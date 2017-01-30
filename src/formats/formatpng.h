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

class cFormatPng final : public cFormat
{
public:
    cFormatPng(const char* lib, iCallbacks* callbacks);
    ~cFormatPng();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
