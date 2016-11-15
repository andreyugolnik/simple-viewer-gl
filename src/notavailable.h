/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "formats/format.h"

class CNotAvailable final : public CFormat
{
public:
    CNotAvailable();
    ~CNotAvailable();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
