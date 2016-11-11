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

class CFormatJpeg final : public CFormat
{
public:
    CFormatJpeg(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~CFormatJpeg();

    virtual bool Load(const char* filename, sBitmapDescription& desc) override;
};
