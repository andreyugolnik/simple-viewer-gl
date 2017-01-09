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

class cFormatWebP final : public CFormat
{
public:
    cFormatWebP(const char* lib, const char* name, iCallbacks* callbacks);
    ~cFormatWebP();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
