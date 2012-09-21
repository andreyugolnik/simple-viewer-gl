////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 26-Aug-2010
// changed: 21.09.2012-Apr-2011
// version: 0.0.0.5
//
////////////////////////////////////////////////

#ifndef FORMATPNG_H
#define FORMATPNG_H

#include "format.h"

class CFormatPng : public CFormat
{
public:
    CFormatPng(Callback callback, const char* _lib, const char* _name);
    virtual ~CFormatPng();

    virtual bool Load(const char* filename, int subImage = 0);

private:
    void* m_lib;
};

#endif // FORMATPNG_H

