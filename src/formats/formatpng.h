////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 26-Aug-2010
// changed: 24-Apr-2011
// version: 0.0.0.3
//
////////////////////////////////////////////////

#ifndef FORMATPNG_H
#define FORMATPNG_H

#include "format.h"

class CFormatPng : public CFormat
{
public:
    CFormatPng(Callback callback);
    virtual ~CFormatPng();

    virtual bool Load(const char* filename, int subImage = 0);

private:
};

#endif // FORMATPNG_H

