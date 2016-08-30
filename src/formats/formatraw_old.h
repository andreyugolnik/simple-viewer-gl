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

class cFormatRawOld final : public CFormat
{
public:
    cFormatRawOld(const char* lib, const char* name);
    ~cFormatRawOld();

    bool Load(const char* filename, unsigned subImage);
    bool isSupported(cFile& file, Buffer& buffer) const;
    //bool isRawFormat(const char* name);
};

