/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#if !defined(IMLIB2_SUPPORT)

#include "format.h"

class cFormatBmp final : public cFormat
{
public:
    cFormatBmp(const char* lib, iCallbacks* callbacks);
    ~cFormatBmp();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};

#endif
