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

class cFormatScr final : public cFormat
{
public:
    cFormatScr(iCallbacks* callbacks);
    ~cFormatScr();

    bool isSupported(cFile& file, Buffer& buffer) const override;

private:
    bool LoadImpl(const char* filename, sBitmapDescription& desc) override;

private:
    void loadScr(const unsigned char* buffer, sBitmapDescription& desc) const;
    void loadScB(const unsigned char* buffer, sBitmapDescription& desc) const;
    void loadAtr(const unsigned char* buffer, sBitmapDescription& desc) const;
    void loadMcX(const unsigned char* buffer, sBitmapDescription& desc, unsigned X) const;
    void loadBMcX(const unsigned char* buffer, sBitmapDescription& desc, unsigned X) const;
};
