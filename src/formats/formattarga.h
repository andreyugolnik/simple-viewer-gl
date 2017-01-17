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

class cFormatTarga final : public CFormat
{
public:
    cFormatTarga(const char* lib, const char* name, iCallbacks* callbacks);
    ~cFormatTarga();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
