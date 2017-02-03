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

    bool isSupported(cFile& /*file*/, Buffer& /*buffer*/) const override
    {
        return true;
    }

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;
};
