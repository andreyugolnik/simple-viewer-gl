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

#include <string>

class cFile;
struct IcoDirentry;

class cFormatIco final : public cFormat
{
public:
    cFormatIco(const char* lib, iCallbacks* callbacks);
    ~cFormatIco();

private:
    virtual bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
    virtual bool LoadSubImageImpl(unsigned current, sBitmapDescription& desc) override;

    bool load(unsigned current, sBitmapDescription& desc);
    bool loadOrdinaryFrame(sBitmapDescription& desc, cFile& file, const IcoDirentry* image);
    bool loadPngFrame(sBitmapDescription& desc, cFile& file, const IcoDirentry* image);
    int calcIcoPitch(unsigned bppImage, unsigned width);
    int getBit(const uint8_t* data, int bit, unsigned width);
    int getNibble(const uint8_t* data, int nibble, unsigned width);
    int getByte(const uint8_t* data, int byte, unsigned width);

private:
    std::string m_filename;
};
