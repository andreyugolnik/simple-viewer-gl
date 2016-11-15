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

struct X10WindowDump;
struct X11WindowDump;

class cFile;

class CFormatXwd final : public CFormat
{
public:
    CFormatXwd(const char* lib, const char* name, iCallbacks* callbacks);
    ~CFormatXwd();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;

    bool loadX10(const X10WindowDump& header, cFile& file, sBitmapDescription& desc);
    bool loadX11(const X11WindowDump& header, cFile& file, sBitmapDescription& desc);
};
