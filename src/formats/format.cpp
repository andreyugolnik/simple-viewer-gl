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

#include <assert.h>

CFormat::CFormat(const char* lib, const char* type)
    : m_callbacks(0)
    , m_percent(-1)
    , m_type(type)
    , m_lib(0)
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
    if(lib)
    {
        std::string path(lib);
#if defined(__linux__)
        path += ".so";
        m_lib = dlopen(path.c_str(), RTLD_LAZY);
        if(m_lib)
        {
            std::cout << type << " format supported." << std::endl;
        }
        else
        {
            std::cout << "(WW) " << type << " format unsupported: " << dlerror() << std::endl;
        }
#else
        path += ".dylib";
#endif
    }
    else
    {
        std::cout << type << " format supported." << std::endl;
    }
}

CFormat::~CFormat()
{
    FreeMemory();
    dlclose(m_lib);
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

uint16_t read_uint16(uint8_t* p)
{
    return (p[0] << 8) | p[1];
}

uint32_t read_uint32(uint8_t* p)
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

void swap_uint32s(uint8_t* p, uint32_t size)
{
    const uint32_t items = size / 4;
    for(uint32_t i = 0; i < items; i++)
    {
        *((uint32_t*)p) = read_uint32(p);
        p += 4;
    }
}

