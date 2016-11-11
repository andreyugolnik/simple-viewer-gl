/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "format.h"

struct GifColorType;

class CFormatGif final : public CFormat
{
public:
    CFormatGif(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatGif();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    void putPixel(int pos, const GifColorType* color, bool transparent);
};
