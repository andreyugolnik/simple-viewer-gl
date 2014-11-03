/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATPPM_H_4GX8MCRJ
#define FORMATPPM_H_4GX8MCRJ

#include "format.h"

class cFormatPpm : public CFormat
{
public:
    cFormatPpm(Callback callback, const char* lib, const char* name);
    virtual ~cFormatPpm();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    bool readAscii(int w, int h, int bpp);
    bool readRaw(int w, int h, int bpp);
};

#endif /* end of include guard: FORMATPPM_H_4GX8MCRJ */

