/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATJPEG_H
#define FORMATJPEG_H

#include "format.h"

class CFormatJpeg : public CFormat
{
public:
    CFormatJpeg(const char* lib, const char* name);
    virtual ~CFormatJpeg();

    virtual bool Load(const char* filename, unsigned subImage = 0);
};

#endif // FORMATJPEG_H

