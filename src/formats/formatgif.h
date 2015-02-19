/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATGIF_H
#define FORMATGIF_H

#include "format.h"

struct GifColorType;

class CFormatGif : public CFormat
{
public:
    CFormatGif(const char* lib, const char* name);
    virtual ~CFormatGif();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    void putPixel(int pos, const GifColorType* color, bool transparent);
};

#endif // FORMATGIF_H

