/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "common/buffer.h"
#include "cms/cms.h"

class iCallbacks;
class cFile;
struct sBitmapDescription;

class cFormat
{
public:
    virtual ~cFormat();

    virtual bool isSupported(cFile& file, Buffer& buffer) const = 0;

    bool Load(const char* filename, sBitmapDescription& desc);
    bool LoadSubImage(unsigned subImage, sBitmapDescription& desc);

    void updateProgress(float percent) const;

    const char* getFormatName() const
    {
        return m_formatName;
    }

    virtual void stop()
    {
        m_stop = true;
    }

    virtual void dump(sBitmapDescription& desc) const;

protected:
    cFormat(iCallbacks* callbacks);
    bool readBuffer(cFile& file, Buffer& buffer, unsigned minSize) const;

private:
    virtual bool LoadImpl(const char* filename, sBitmapDescription& desc) = 0;
    virtual bool LoadSubImageImpl(unsigned /*subImage*/, sBitmapDescription& /*desc*/)
    {
        return false;
    }

private:
    iCallbacks* m_callbacks;

protected:
    cCMS m_cms;

    const char* m_formatName = nullptr;
    bool m_stop = false;
};
