/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatpvr.h"
#include "file_zlib.h"
#include "helpers.h"

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

static bool isZpvr(cFile& file)
{
    uint8_t header[4];
    if(sizeof(header) == file.read(header, sizeof(header)))
    {
        if(::memcmp(header, "ZPVR", 4) == 0)
        {
            return true;
        }
    }
    return false;
}

bool cFormatPvr::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    if(isZpvr(file))
    {
        cFileZlib zip(&file);
        return readPvr(zip);
    }
    return readPvr(file);
}

bool cFormatPvr::readPvr(cFileInterface& file)
{
    PVRTexHeader header;
    if(sizeof(header) != file.read(&header, sizeof(header)))
    {
        printf("Can't read PVR header.\n");
        return false;
    }

    //printf("version: %u\n", header.version);
    //printf("flags: %u\n", header.flags);
    //printf("format: %u\n", header.pixels_format);
    //printf("format: %u\n", header.pixels_format2);
    //printf("colorspace: %u\n", header.colorspace);
    //printf("channel_type: %u\n", header.channel_type);
    //printf("height: %u\n", header.height);
    //printf("width: %u\n", header.width);
    //printf("depth: %u\n", header.depth);
    //printf("num_surfaces: %u\n", header.num_surfaces);
    //printf("num_faces: %u\n", header.num_faces);
    //printf("mipmap_count: %u\n", header.mipmap_count);
    //printf("metadata_size: %u\n", header.metadata_size);

    if(header.metadata_size > 0)
    {
        file.seek(header.metadata_size, SEEK_CUR);
    }

    unsigned bytes = 0;

    const uint64_t pixelFormat = ((uint64_t)header.pixels_format2 << 32 | header.pixels_format);
    switch(pixelFormat)
    {
    case (uint64_t)RGBA8888:
        bytes    = 4;
        m_format = GL_RGBA;
        break;

    case (uint64_t)RGBA4444:
        bytes    = 2;
        m_format = GL_UNSIGNED_SHORT_4_4_4_4;
        break;

    case (uint64_t)RGB565:
        bytes    = 2;
        m_format = GL_UNSIGNED_SHORT_5_6_5;
        break;

    case (uint64_t)RGBA5551:
        bytes    = 2;
        m_format = GL_UNSIGNED_SHORT_5_5_5_1;
        break;

    default:
        printf("Unsupported format.\n");
        return false;
    }

    m_bpp      = bytes * 8;
    m_bppImage = bytes * 8;
    m_width    = header.width;
    m_height   = header.height;
    m_pitch    = m_width * bytes;

    const unsigned size = m_pitch * m_height;
    m_bitmap.resize(size);
    if(size != file.read(&m_bitmap[0], size))
    {
        printf("Unexpected EOF.\n");
        return false;
    }

    return true;
}

