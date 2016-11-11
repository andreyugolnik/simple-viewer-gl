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

class cFormatScr final : public CFormat
{
public:
    cFormatScr(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~cFormatScr();

    virtual bool Load(const char* filename, unsigned subImage = 0) override;
};

