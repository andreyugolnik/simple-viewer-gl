/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATTIFF_H
#define FORMATTIFF_H

#include "format.h"

class CFormatTiff : public CFormat
{
public:
    CFormatTiff(const char* lib, const char* name);
    virtual ~CFormatTiff();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
};

#endif // FORMATTIFF_H

