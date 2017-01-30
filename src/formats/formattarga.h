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

class cFormatTarga final : public cFormat
{
public:
    cFormatTarga(const char* lib, iCallbacks* callbacks);
    ~cFormatTarga();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};