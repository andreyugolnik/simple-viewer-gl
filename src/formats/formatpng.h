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

class CFormatPng final : public CFormat
{
public:
    CFormatPng(const char* lib, const char* name, iCallbacks* callbacks);
    ~CFormatPng();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
