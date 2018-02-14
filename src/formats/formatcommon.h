/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#if defined(IMLIB2_SUPPORT)

#include "format.h"

class cFormatCommon final : public cFormat
{
public:
    explicit cFormatCommon(iCallbacks* callbacks);
    ~cFormatCommon();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};

#endif
