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
    CFormatGif(Callback callback, const char* _lib, const char* _name);
    virtual ~CFormatGif();

    virtual bool Load(const char* filename, int subImage = 0);

private:
    void putPixel(int _pos, const GifColorType* _color, bool _transparent);
};

#endif // FORMATGIF_H

