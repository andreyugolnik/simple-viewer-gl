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
    CFormatJpeg(Callback callback, const char* _lib, const char* _name);
    virtual ~CFormatJpeg();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
};

#endif // FORMATJPEG_H
