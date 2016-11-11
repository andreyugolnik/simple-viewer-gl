/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATTIFF_H
#define FORMATTIFF_H

#include "format.h"

class CFormatTiff : public CFormat
{
public:
    CFormatTiff(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatTiff();

    virtual bool Load(const char* filename, unsigned subImage = 0);
};

#endif // FORMATTIFF_H

