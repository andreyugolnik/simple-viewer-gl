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

class CFormatCommon final : public CFormat
{
public:
    CFormatCommon(const char* lib, const char* name, iCallbacks* callbacks);
    ~CFormatCommon();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};

#endif
