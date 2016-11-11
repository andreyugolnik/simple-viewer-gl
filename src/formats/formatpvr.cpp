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

#include <cstring>

enum PVRPixelFormat
{
    RGBPVRTC2  = 0,
    RGBAPVRTC2 = 1,
    RGBPVRTC4  = 2,
    RGBAPVRTC4 = 3,

    PREMULTIPLIEDALPHAMASK = 0x2,
    FORMATMASK             = 0xFFFFFFFF00000000,

    RGBA8888               = 0x0808080861626772,
    BGRA8888               = 0x0808080861726762,
    RGB888                 = 0x0008080800626772,
    RGBA4444               = 0x0404040461626772,
    ARGB4444               = 0x0404040462677261,
    RGB565                 = 0x0005060500626772,
    RGBA5551               = 0x0105050561626772,
    ARGB1555               = 0x0505050162677261,
    A8                     = 0x0800000061,
    LA8                    = 0x08080000616C,
    L8                     = 0x00080000006C,
};

//enum PVRConversionFlags
//{
    //None,
    //RGBA44442ARGB4444 = 0x1,
    //RGBA44442RGBA8888 = 0x2,
    //RGB5652RGBA8888   = 0x4,
    //RGBA55512ARGB1555 = 0x8,
    //RGBA55512RGBA8888 = 0x10,
    //RGBA88882BGRA8888 = 0x20

//};

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

//static void ConvertRgba4444ToRgba8888(Buffer& buffer, unsigned width, unsigned height)
//{
    //Buffer rgba(width * height * 4);

    //uint16_t* in = (uint16_t*)&buffer[0];
    //uint32_t* out = (uint32_t*)&rgba[0];

    //for(unsigned i = 0, size = width * height; i < size; i++)
    //{
        //const uint16_t value = in[i];

        //const uint32_t r = ((value >> 12) & 0xF) * 17;
        //const uint32_t g = (((value >> 8) & 0xF) * 17) << 8;
        //const uint32_t b = (((value >> 4) & 0xF) * 17) << 16;
        //const uint32_t a = ((value & 0xF) * 17) << 24;

        //out[i] = r | g | b | a;
    //}

    //std::swap(buffer, rgba);
//}

//static void ConvertRgb565ToRgba8888(Buffer& buffer, unsigned width, unsigned height)
//{
    //Buffer rgba(width * height * 4);

    //uint16_t* in = (uint16_t*)&buffer[0];
    //uint32_t* out = (uint32_t*)&rgba[0];

    //for(unsigned i = 0, size = width * height; i < size; i++)
    //{
        //uint16_t value = in[i];

        //uint32_t r = value & 0xF800;
        //r = (r >> 8 | r >> 13);
        //uint32_t g = value & 0x7E0;
        //g = (g >> 3 | g >> 9) << 8;
        //uint32_t b = value & 0x1F;
        //b = (b << 3 | b >> 2) << 16;
        //const uint32_t a = 0xFF << 24;

        //out[i] = r | g | b | a;
    //}

    //std::swap(buffer, rgba);
//}

//static void ConvertRgba5551ToArgb1555Inplace(Buffer& buffer, unsigned width, unsigned height)
//{
    //uint16_t* inOut = (uint16_t*)&buffer[0];
    //for(unsigned i = 0, size = width * height; i < size; i++)
    //{
        //uint16_t value = inOut[i];
        //value = (uint16_t)(((value >> 1) & 0x7FFF) | (value << 15));
        //inOut[i] = value;
    //}
//}

//static void ConvertRgba4444ToArgb4444Inplace(Buffer& buffer, unsigned width, unsigned height)
//{
    //uint16_t* inOut = (uint16_t*)&buffer[0];
    //for(unsigned i = 0, size = width * height; i < size; i++)
    //{
        //uint16_t value = inOut[i];
        //value = (uint16_t)(((value >> 4) & 0x0FFF) | (value << 12));
        //inOut[i] = value;
    //}
//}

//static void ConvertRgba5551ToRgba8888(Buffer& buffer, unsigned width, unsigned height)
//{
    //Buffer rgba(width * height * 4);

    //uint16_t* in = (uint16_t*)&buffer[0];
    //uint32_t* out = (uint32_t*)&rgba[0];

    //for(unsigned i = 0, size = width * height; i < size; i++)
    //{
        //uint16_t value = in[i];
        //uint32_t r = (value & 0xF800);
        //r = (r >> 8 | r >> 13);
        //uint32_t g = (value & 0x7C0);
        //g = (g << 5 | g) & 0xFF00;
        //uint32_t b = (value & 0x3E);
        //b = (b << 18 | b << 13) & 0xFF0000;
        //uint32_t a = ((value & 0x1) * 255) << 24;

        //out[i] = r | g | b | a;
    //}

    //std::swap(buffer, rgba);
//}



cFormatPvr::cFormatPvr(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
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

bool cFormatPvr::isSupported(cFile& file, Buffer& buffer) const
{
    if(!readBuffer(file, buffer, 4))
    {
        return false;
    }

    return (::memcmp(&buffer[0], "ZPVR", 4) == 0 || ::memcmp(&buffer[0], "PVR", 3) == 0);
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
    file.seek(0, SEEK_SET);
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

    printf("version: %u\n", header.version);
    printf("flags: %u\n", header.flags);
    printf("format: 0x%x\n", header.pixels_format);
    printf("format: 0x%x\n", header.pixels_format2);
    printf("colorspace: %u\n", header.colorspace);
    printf("channel_type: %u\n", header.channel_type);
    printf("height: %u\n", header.height);
    printf("width: %u\n", header.width);
    printf("depth: %u\n", header.depth);
    printf("num_surfaces: %u\n", header.num_surfaces);
    printf("num_faces: %u\n", header.num_faces);
    printf("mipmap_count: %u\n", header.mipmap_count);
    printf("metadata_size: %u\n", header.metadata_size);

    if(header.metadata_size > 0)
    {
        std::vector<char> dummy(header.metadata_size);
        file.read(&dummy[0], dummy.size());
    }

    unsigned bytes = 0;

    const uint64_t pixelFormat = ((uint64_t)header.pixels_format2 << 32 | header.pixels_format);
    if((pixelFormat & PVRPixelFormat::FORMATMASK) != 0)
    {
        switch(pixelFormat)
        {
        case (uint64_t)PVRPixelFormat::RGB888:
            bytes    = 3;
            m_format = GL_RGB;
            break;
        case (uint64_t)PVRPixelFormat::RGBA8888:
            bytes    = 4;
            m_format = GL_RGBA;
            break;
        case (uint64_t)PVRPixelFormat::BGRA8888:
            bytes    = 4;
            m_format = GL_BGRA;
            break;
        case (uint64_t)PVRPixelFormat::RGB565:
            bytes    = 2;
            m_format = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case (uint64_t)PVRPixelFormat::RGBA4444:
            bytes    = 2;
            m_format = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case (uint64_t)PVRPixelFormat::ARGB4444:
            bytes    = 2;
            m_format = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case (uint64_t)PVRPixelFormat::RGBA5551:
            bytes    = 2;
            m_format = GL_UNSIGNED_SHORT_5_5_5_1;
            break;
        case (uint64_t)PVRPixelFormat::ARGB1555:
            bytes    = 2;
            m_format = GL_UNSIGNED_SHORT_5_5_5_1;
            break;
        case (uint64_t)PVRPixelFormat::LA8:
            bytes    = 2;
            m_format = GL_LUMINANCE_ALPHA;
            break;
        case (uint64_t)PVRPixelFormat::A8:
            bytes    = 1;
            m_format = GL_ALPHA;
            break;
        case (uint64_t)PVRPixelFormat::L8:
            bytes    = 1;
            m_format = GL_LUMINANCE;
            break;
        }
    }
    else
    {
        printf("Unsupported format.\n");
        return false;

        //const PVRPixelFormat format = (PVRPixelFormat)pixelFormat;
        //switch(format)
        //{
        //case PVRPixelFormat::RGBPVRTC2:
            //printf("SurfaceFormat.RgbPvrtc2\n");
            //break;
        //case PVRPixelFormat::RGBAPVRTC2:
            //printf("SurfaceFormat.RgbaPvrtc2\n");
            //break;
        //case PVRPixelFormat::RGBPVRTC4:
            //printf("SurfaceFormat.RgbPvrtc4\n");
            //break;
        //case PVRPixelFormat::RGBAPVRTC4:
            //printf("SurfaceFormat.RgbaPvrtc4\n");
            //break;
        //}
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

    //if((pixelFormat & PVRPixelFormat::FORMATMASK) != 0)
    //{
        //const uint32_t flags = header.flags;
        //switch(pixelFormat)
        //{
        //case (uint64_t)PVRPixelFormat::RGBA4444:
            //if((flags & PVRConversionFlags::RGBA44442ARGB4444) != 0)
            //{
                ////ConvertRgba4444ToArgb4444Inplace(&m_bitmap[0], header.width, header.height);
                ////m_format = GL_UNSIGNED_SHORT_4_4_4_4;
            //}
            //if((flags & PVRConversionFlags::RGBA44442RGBA8888) != 0)
            //{
                ////ConvertRgba4444ToRgba8888(m_bitmap, header.width, header.height);
                ////m_bpp    = 32;
                ////m_pitch  = m_width * 4;
                ////m_format = GL_RGBA;
            //}
            //break;

        //case (uint64_t)PVRPixelFormat::RGB565:
            //if((flags & PVRConversionFlags::RGB5652RGBA8888) != 0)
            //{
                ////_dataPointer = ConvertRgb565ToRgba8888(&m_bitmap[0], header.width, header.height);
                ////m_format = GL_RGBA;
            //}
            //break;

        //case (uint64_t)PVRPixelFormat::RGBA5551:
            //if((flags & PVRConversionFlags::RGBA55512ARGB1555) != 0)
            //{
                ////ConvertRgba5551ToArgb1555Inplace(&m_bitmap[0], header.width, header.height);
                ////m_format = GL_UNSIGNED_SHORT_5_5_5_1;
            //}
            //if((flags & PVRConversionFlags::RGBA55512RGBA8888) != 0)
            //{
                ////_dataPointer = ConvertRgba5551ToRgba8888(&m_bitmap[0], header.width, header.height);
                ////m_format = GL_RGBA;
            //}
            //break;
        //}
    //}

    return true;
}

