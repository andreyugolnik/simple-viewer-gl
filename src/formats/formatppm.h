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

class cFormatPpm : public CFormat
{
public:
    cFormatPpm(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~cFormatPpm();

    virtual bool Load(const char* filename, sBitmapDescription& desc) override;

private:
    bool readAscii1(cFile& file, int w, int h, sBitmapDescription& desc);
    bool readRaw1(cFile& file, int w, int h, sBitmapDescription& desc);

    bool readAscii8(cFile& file, int w, int h, sBitmapDescription& desc);
    bool readRaw8(cFile& file, int w, int h, sBitmapDescription& desc);

    bool readAscii24(cFile& file, int w, int h, sBitmapDescription& desc);
    bool readRaw24(cFile& file, int w, int h, sBitmapDescription& desc);
};
