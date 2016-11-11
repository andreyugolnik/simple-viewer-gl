/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATPPM_H_4GX8MCRJ
#define FORMATPPM_H_4GX8MCRJ

#include "format.h"

class cFile;

class cFormatPpm : public CFormat
{
public:
    cFormatPpm(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~cFormatPpm();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    bool readAscii1(cFile& file, int w, int h);
    bool readRaw1(cFile& file, int w, int h);

    bool readAscii8(cFile& file, int w, int h);
    bool readRaw8(cFile& file, int w, int h);

    bool readAscii24(cFile& file, int w, int h);
    bool readRaw24(cFile& file, int w, int h);
};

#endif /* end of include guard: FORMATPPM_H_4GX8MCRJ */

