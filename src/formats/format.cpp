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

#include <cassert>
#include <dlfcn.h>
#include <iostream>
#include <string>

CFormat::CFormat(const char* libName, const char* formatName, iCallbacks* callbacks)
    : m_formatName(formatName)
    , m_callbacks(callbacks)
{
    if (libName != nullptr)
    {
        std::string path(libName);
#if defined(__linux__)
        path += ".so";
#else
        path += ".dylib";
#endif
        m_lib = dlopen(path.c_str(), RTLD_LAZY);
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

CFormat::~CFormat()
{
    if (m_lib != nullptr)
    {
        dlclose(m_lib);
    }
}

void CFormat::dumpFormat()
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

void CFormat::updateProgress(float percent)
{
    assert(m_callbacks);
    if (m_percent != percent)
    {
        m_percent = percent;
        m_callbacks->doProgress(percent);
    }
}
