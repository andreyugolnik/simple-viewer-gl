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
    virtual ~CFormatCommon();

    virtual bool Load(const char* filename, unsigned subImage = 0);
    virtual void FreeMemory();
};

#endif
