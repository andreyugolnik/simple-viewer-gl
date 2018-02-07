/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types/types.h"

#include <functional>

class cCMS;
class cFile;
struct sBitmapDescription;

class cPngReader
{
public:
    cPngReader(cCMS& cms);
    virtual ~cPngReader();

    typedef std::function<void (float percent)> progressCallback;

    void setProgressCallback(progressCallback callback)
    {
        m_progress = callback;
    }

    bool isValid(const uint8_t* data, uint32_t size) const;

    bool loadPng(sBitmapDescription& desc, const uint8_t* data, uint32_t size) const;
    bool loadPng(sBitmapDescription& desc, cFile& file) const;

private:
    void updateProgress(float percent) const
    {
        if (m_progress != nullptr)
        {
            m_progress(percent);
        }
    }

private:
    cCMS& m_cms;
    progressCallback m_progress = nullptr;
};
