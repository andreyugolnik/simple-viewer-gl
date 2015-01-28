/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATJPEG_H
#define FORMATJPEG_H

#include "format.h"

class CFormatJpeg : public CFormat
{
public:
    CFormatJpeg(const char* lib, const char* name);
    virtual ~CFormatJpeg();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
};

#endif // FORMATJPEG_H
