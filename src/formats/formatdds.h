/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATDDS_H
#define FORMATDDS_H

#include "format.h"

class CFormatDds : public CFormat
{
public:
    CFormatDds(const char* lib, const char* name);
    virtual ~CFormatDds();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
};

#endif // FORMATDDS_H

