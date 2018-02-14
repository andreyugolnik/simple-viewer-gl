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

class cFormatTarga final : public cFormat
{
public:
    explicit cFormatTarga(iCallbacks* callbacks);
    ~cFormatTarga();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
