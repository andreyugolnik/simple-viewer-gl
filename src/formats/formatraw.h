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

class cFormatRaw final : public CFormat
{
public:
    cFormatRaw(const char* lib, const char* name, iCallbacks* callbacks);
    ~cFormatRaw();

    bool Load(const char* filename, unsigned subImage);
    bool isSupported(cFile& file, Buffer& buffer) const;
    //bool isRawFormat(const char* name);
};

