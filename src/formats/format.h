/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../common/buffer.h"

#include <GLFW/glfw3.h>

//#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

class iCallbacks;
class cFile;
struct sBitmapDescription;

class CFormat
{
public:
    virtual ~CFormat();

    virtual bool isSupported(cFile& /*file*/, Buffer& /*buffer*/) const
    {
        return false;
    }

    virtual bool Load(const char* filename, sBitmapDescription& desc) = 0;
    virtual bool LoadSubImage(unsigned /*subImage*/, sBitmapDescription& /*desc*/)
    {
        return false;
    }

    void updateProgress(float percent);

    const char* getFormatName() const
    {
        return m_formatName;
    }

    virtual void dumpFormat();

protected:
    CFormat(const char* libName, const char* formatName, iCallbacks* callbacks);

private:
    const char* m_formatName;
    iCallbacks* m_callbacks;
    float m_percent = 0.0f;

protected:
    void* m_lib = nullptr;

    enum class eSupport
    {
        Unsupported,
        ExternalLib,
        Internal
    };
    eSupport m_support = eSupport::Unsupported;
};
