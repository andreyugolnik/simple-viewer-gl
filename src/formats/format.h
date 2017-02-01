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
#include "../cms/cms.h"

class iCallbacks;
class cFile;
struct sBitmapDescription;

class cFormat
{
public:
    virtual ~cFormat();

    virtual bool isSupported(cFile& /*file*/, Buffer& /*buffer*/) const
    {
        return false;
    }

    bool Load(const char* filename, sBitmapDescription& desc);
    bool LoadSubImage(unsigned subImage, sBitmapDescription& desc);

    void updateProgress(float percent);

    const char* getFormatName() const
    {
        return m_formatName;
    }

    virtual void stop()
    {
        m_stop = true;
    }

    virtual void dumpFormat();

protected:
    cFormat(const char* libName, iCallbacks* callbacks);

private:
    virtual bool LoadImpl(const char* filename, sBitmapDescription& desc) = 0;
    virtual bool LoadSubImageImpl(unsigned /*subImage*/, sBitmapDescription& /*desc*/)
    {
        return false;
    }

private:
    iCallbacks* m_callbacks;
    float m_percent = 0.0f;

protected:
    cCMS m_cms;

    const char* m_formatName = nullptr;
    void* m_lib = nullptr;
    bool m_stop = false;

    enum class eSupport
    {
        Unsupported,
        ExternalLib,
        Internal
    };
    eSupport m_support = eSupport::Unsupported;
};
