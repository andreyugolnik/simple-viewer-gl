/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "format.h"
#include "../common/callbacks.h"
#include "../common/file.h"

#include <cassert>
#include <cstdio>
#include <dlfcn.h>

cFormat::cFormat(const char* libName, iCallbacks* callbacks)
    : m_callbacks(callbacks)
{
    if (libName != nullptr)
    {
        char path[100];
#if defined(__linux__)
        ::snprintf(path, sizeof(path), "%s.so", libName);
#else
        ::snprintf(path, sizeof(path), "%s.dylib", libName);
#endif
        m_lib = dlopen(path, RTLD_LAZY);
        if (m_lib != nullptr)
        {
            m_support = eSupport::ExternalLib;
        }
    }
    else
    {
        m_support = eSupport::Internal;
    }
}

cFormat::~cFormat()
{
    if (m_lib != nullptr)
    {
        dlclose(m_lib);
    }
}

bool cFormat::Load(const char* filename, sBitmapDescription& desc)
{
    m_stop = false;
    return LoadImpl(filename, desc);
}

bool cFormat::LoadSubImage(unsigned subImage, sBitmapDescription& desc)
{
    m_stop = false;
    return LoadSubImageImpl(subImage, desc);
}

void cFormat::dumpFormat()
{
    switch (m_support)
    {
    case eSupport::Unsupported:
        printf("(WW) %s format unsupported.\n", m_formatName);
        break;

    case eSupport::ExternalLib:
        printf("%s format supported by external lib.\n", m_formatName);
        break;

    case eSupport::Internal:
        printf("%s format has internal support.\n", m_formatName);
        break;
    }
}

void cFormat::updateProgress(float percent)
{
    assert(m_callbacks != nullptr);
    if (m_percent != percent)
    {
        m_percent = percent;
        m_callbacks->doProgress(percent);
    }
}

bool cFormat::readBuffer(cFile& file, Buffer& buffer, uint32_t minSize) const
{
    const uint32_t size = buffer.size();
    if (size < minSize)
    {
        buffer.resize(minSize);
        const uint32_t length = minSize - size;
        if (length != file.read(&buffer[size], length))
        {
            return false;
        }
    }

    return minSize <= buffer.size();
}
