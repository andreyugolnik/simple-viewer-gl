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

class cFormatPnm final : public cFormat
{
public:
    explicit cFormatPnm(iCallbacks* callbacks);
    ~cFormatPnm();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
