/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "format.h"
#include "../callbacks.h"

#include <iostream>
#include <dlfcn.h>
#include <cassert>

CFormat::CFormat(const char* libName, const char* formatName)
    : m_callbacks(nullptr)
    , m_percent(-1)
    , m_formatName(formatName)
    , m_lib(nullptr)
    , m_support(eSupport::Unsupported)
    , m_format(GL_RGB)
    , m_width(0)
    , m_height(0)
    , m_pitch(0)
    , m_bpp(0)
    , m_bppImage(0)
    , m_size(-1) // -1 mean that file can't be opened
    , m_subImage(0)
    , m_subCount(0)
{
    if(libName != nullptr)
    {
        std::string path(libName);
#if defined(__linux__)
        path += ".so";
#else
        path += ".dylib";
#endif
        m_lib = dlopen(path.c_str(), RTLD_LAZY);
        if(m_lib != nullptr)
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
    FreeMemory();
    if(m_lib != nullptr)
    {
        dlclose(m_lib);
    }
}

void CFormat::dumpFormat()
{
    const char* formatName = m_formatName.c_str();
    switch(m_support)
    {
    case eSupport::Unsupported:
        printf("(WW) %s format unsupported.\n", formatName);
        break;

    case eSupport::ExternalLib:
        printf("%s format supported by external lib.\n", formatName);
        break;

    case eSupport::Internal:
        printf("%s format has internal support.\n", formatName);
        break;
    }
}

void CFormat::FreeMemory()
{
    m_bitmap.clear();
}

void CFormat::progress(int percent)
{
    assert(m_callbacks);
    if(m_percent != percent)
    {
        m_percent = percent;
        m_callbacks->doProgress(percent);
    }
}

void CFormat::reset()
{
    m_format   = GL_RGB;
    m_width    = 0;
    m_height   = 0;
    m_pitch    = 0;
    m_bpp      = 0;
    m_bppImage = 0;
    m_size     = -1;
    m_subImage = 0;
    m_subCount = 0;
    m_info.clear();

    FreeMemory();
}

