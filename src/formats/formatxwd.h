/**********************************************\
*
*  Andrey A. Ugolnik
*  Tiny Orange
*  http://www.tinyorange.com
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATXWD_H
#define FORMATXWD_H

#include "format.h"

struct X10WindowDump;
struct X11WindowDump;

class CFormatXwd : public CFormat
{
public:
    CFormatXwd(const char* lib, const char* name);
    virtual ~CFormatXwd();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    bool loadX10(const X10WindowDump& header, cFile& file);
    bool loadX11(const X11WindowDump& header, cFile& file);
};

#endif // FORMATXWD_H

