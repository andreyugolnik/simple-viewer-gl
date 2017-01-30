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

class cNotAvailable final : public cFormat
{
public:
    cNotAvailable();
    ~cNotAvailable();

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
