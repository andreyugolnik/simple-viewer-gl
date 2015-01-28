/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATGIF_H
#define FORMATGIF_H

#include "format.h"
#include <gif_lib.h>

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

