////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#ifndef FORMATPNG_H
#define FORMATPNG_H

#include "format.h"

class CFormatPng : public CFormat
{
public:
    CFormatPng(const char* lib, const char* name);
    virtual ~CFormatPng();

    virtual bool Load(const char* filename, unsigned subImage = 0);
};

#endif // FORMATPNG_H

