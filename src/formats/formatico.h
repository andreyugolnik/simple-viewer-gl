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

class cFile;
struct IcoDirentry;

class CFormatIco final : public CFormat
{
public:
    CFormatIco(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatIco();

    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    bool loadOrdinaryFrame(cFile& file, const IcoDirentry* image);
    bool loadPngFrame(cFile& file, const IcoDirentry* image);
    int calcIcoPitch();
    int getBit(const uint8_t* data, int bit);
    int getNibble(const uint8_t* data, int nibble);
    int getByte(const uint8_t* data, int byte);
};
