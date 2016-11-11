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

class cFileInterface;

class cFormatPvr final : public CFormat
{
public:
    cFormatPvr(const char* lib, const char* name, iCallbacks* callbacks);
    ~cFormatPvr();

    virtual bool isSupported(cFile& file, Buffer& buffer) const override;
    virtual bool Load(const char* filename, sBitmapDescription& desc) override;

private:
    bool readPvr(cFileInterface& file, sBitmapDescription& desc);
};
