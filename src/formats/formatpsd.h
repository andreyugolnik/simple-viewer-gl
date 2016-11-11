/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATPSD_H
#define FORMATPSD_H

#include "format.h"

class CFormatPsd : public CFormat
{
public:
    CFormatPsd(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatPsd();

    virtual bool Load(const char* filename, unsigned subImage = 0);
};

#endif // FORMATPSD_H

