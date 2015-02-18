/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatpvr.h"

enum PVRPixelFormat
{
    RgbPvrtc2  = 0,
    RgbaPvrtc2 = 1,
    RgbPvrtc4  = 2,
    RgbaPvrtc4 = 3,

    RGBA4444   = 0x0404040461626772,
    RGBA8888   = 0x0808080861626772,
    RGB565     = 0x0005060500626772,
    RGBA5551   = 0x0105050561626772,
    A8         = 0x0800000061,
};

enum PVRConversionFlags
{
    None,
    RGBA44442ARGB4444 = 0x1,
    RGBA44442RGBA8888 = 0x2,
    RGB5652RGBA8888   = 0x4
};

struct PVRTexHeader
{
    uint32_t version;
    uint32_t flags;
    uint32_t pixels_format;
    uint32_t pixels_format2;
    uint32_t colorspace;
    uint32_t channel_type;
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t num_surfaces;
    uint32_t num_faces;
    uint32_t mipmap_count;
    uint32_t metadata_size;
};

cFormatPvr::cFormatPvr(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

cFormatPvr::~cFormatPvr()
{
}

bool cFormatPvr::Load(const char* filename, unsigned subImage)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    PVRTexHeader header;
    if(sizeof(header) != file.read(&header, sizeof(header)))
    {
        printf("Can't read PVR header.\n");
        return false;
    }

    if(header.metadata_size > 0)
    {
        uint8_t* buff = new uint8_t[header.metadata_size];
        file.read(buff, header.metadata_size);
        delete[] buff;
    }

    uint64_t pixelFormat = ((uint64_t)header.pixels_format2 << 32 | header.pixels_format);

    switch(pixelFormat)
    {
    case (uint64_t)RGBA8888:
        m_format = GL_RGBA;
        m_bpp = 32;
        m_bppImage = 32;
        m_pitch = header.width * header.height * 4;
        break;

    case (uint64_t)RGBA4444:
        m_format = GL_RGBA;
        m_bppImage = 16;
        m_bpp = 16;
        m_pitch = header.width * header.height * 4;
        break;

    case (uint64_t)RGB565:
        m_format = GL_RGB;
        m_bpp = 16;
        m_bppImage = 16;
        m_pitch = header.width * header.height * 3;
        break;

    case (uint64_t)RGBA5551:
        m_format = GL_RGBA;
        m_bpp = 16;
        m_bppImage = 16;
        m_pitch = header.width * header.height * 4;
        break;
    }

    m_width = header.width;
    m_height = header.height;
    m_bitmap.resize(m_pitch * m_height);

        printf("%u x %u", m_width, m_height);
    return true;
}

