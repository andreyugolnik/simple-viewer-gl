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

CFormat::CFormat(const char* lib, const char* name)
    : m_callbacks(0)
    , m_percent(-1)
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
            std::cout << name << " format supported." << std::endl;
        }
        else
        {
            std::cout << "(WW) " << name << " format unsupported: " << dlerror() << std::endl;
        }
#else
        path += ".dylib";
#endif
    }
    else
    {
        std::cout << name << " format supported." << std::endl;
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

uint16_t CFormat::read_uint16(uint8_t* p)
{
    return (p[0] << 8) | p[1];
}

uint32_t CFormat::read_uint32(uint8_t* p)
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

void CFormat::swap_long(uint8_t* bp, uint32_t n)
{
    uint8_t* ep = bp + n;

    while(bp < ep)
    {
        uint8_t c = bp[3];
        bp[3] = bp[0];
        bp[0] = c;
        c = bp[2];
        bp[2] = bp[1];
        bp[1] = c;
        bp += 4;
    }
}

