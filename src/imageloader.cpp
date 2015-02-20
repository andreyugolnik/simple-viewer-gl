/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageloader.h"
#include "formats/file.h"
#include "formats/formatcommon.h"
#include "formats/formatjpeg.h"
#include "formats/formatpsd.h"
#include "formats/formatpng.h"
#include "formats/formatgif.h"
#include "formats/formatico.h"
#include "formats/formattiff.h"
#include "formats/formatxwd.h"
#include "formats/formatdds.h"
#include "formats/formatraw.h"
#include "formats/formatppm.h"
#include "formats/formatpvr.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <assert.h>

static std::string m_path;

CImageLoader::CImageLoader(iCallbacks* callbacks)
    : m_callbacks(callbacks)
    , m_image(0)
    , m_type(TYPES_COUNT)
{
#if defined(IMLIB2_SUPPORT)
    m_formats[TYPE_COMMON] = new CFormatCommon("libImlib2", "ImLib2");
#endif
    m_formats[TYPE_JPG] = new CFormatJpeg("libjpeg", "jpeg");
    m_formats[TYPE_PSD] = new CFormatPsd(0, "PSD");
    m_formats[TYPE_PNG] = new CFormatPng("libpng", "png");
    m_formats[TYPE_GIF] = new CFormatGif("libgif", "gif");
    m_formats[TYPE_ICO] = new CFormatIco(0, "ico");
    m_formats[TYPE_TIF] = new CFormatTiff("libtiff", "tiff");
    m_formats[TYPE_XWD] = new CFormatXwd(0, "xwd");
    m_formats[TYPE_DDS] = new CFormatDds(0, "dds");
    m_formats[TYPE_RAW] = new cFormatRaw(0, "raw");
    m_formats[TYPE_PPM] = new cFormatPpm(0, "ppm");
    m_formats[TYPE_PVR] = new cFormatPvr(0, "pvr");
}

CImageLoader::~CImageLoader()
{
    for(int i = 0; i < TYPES_COUNT; i++)
    {
        delete m_formats[i];
    }
}

bool CImageLoader::LoadImage(const char* path, unsigned subImage)
{
    if(path != 0)
    {
        if(!m_path.empty() && m_path == path)
        {
            if(!m_image->m_bitmap.empty())
            {
                if(GetSub() != subImage)
                {
                    return m_image->Load(path, subImage);
                }
                return true; // image already loaded
            }
        }

        const eImageType type = getType(path);
        if(m_type != type)
        {
            m_type = type;
            FreeMemory();
        }

        if(type == TYPES_COUNT)
        {
            m_image = 0;
            return false;
        }

        m_path = path;
        m_image = m_formats[type];

        m_image->setCallbacks(m_callbacks);
        const bool result = m_image->Load(path, subImage);
        if(!result)
        {
            FreeMemory();
        }
        return result;
    }

    return false;
}

bool CImageLoader::isLoaded() const
{
    return m_image && !m_image->m_bitmap.empty();
}

unsigned char* CImageLoader::GetBitmap() const
{
    assert(m_image);
    if(!m_image->m_bitmap.empty())
    {
        return &m_image->m_bitmap[0];
    }
    return 0;
}

void CImageLoader::FreeMemory()
{
    if(m_image)
    {
        m_image->FreeMemory();
        m_path.clear();
    }
}

unsigned CImageLoader::GetWidth() const
{
    assert(m_image);
    return m_image->m_width;
}

unsigned CImageLoader::GetHeight() const
{
    assert(m_image);
    return m_image->m_height;
}

unsigned CImageLoader::GetPitch() const
{
    assert(m_image);
    return m_image->m_pitch;
}

unsigned CImageLoader::GetBitmapFormat() const
{
    assert(m_image);
    return m_image->m_format;
}

unsigned CImageLoader::GetBpp() const
{
    assert(m_image);
    return m_image->m_bpp;
}

unsigned CImageLoader::GetImageBpp() const
{
    assert(m_image);
    return m_image->m_bppImage;
}

// file size on disk
long CImageLoader::GetSize() const
{
    assert(m_image);
    return m_image->m_size;
}

size_t CImageLoader::GetSizeMem() const
{
    assert(m_image);
    return m_image->m_bitmap.size();
}

unsigned CImageLoader::GetSub() const
{
    assert(m_image);
    return m_image->m_subImage;
}

unsigned CImageLoader::GetSubCount() const
{
    assert(m_image);
    if(m_image != 0)
    {
        return m_image->m_subCount;
    }
    return 0;
}

const char* CImageLoader::getImageType() const
{
    assert(m_image);
    if(m_image != 0)
    {
        return m_image->m_type.c_str();
    }
    return 0;
}

struct sFormatExt
{
    const char* ext;
    eImageType format;
};

eImageType CImageLoader::getType(const char* name)
{
    cFile file;
    if(!file.open(name))
    {
        return TYPES_COUNT;
    }

    Buffer buffer;
    if(m_formats[TYPE_RAW]->isSupported(file, buffer))
    {
        return TYPE_RAW;
    }
    if(m_formats[TYPE_PVR]->isSupported(file, buffer))
    {
        return TYPE_PVR;
    }

    std::string s(name);

    // skip file without extension
    const size_t pos = s.find_last_of('.');
    if(std::string::npos != pos)
    {
        // skip non image file (detect by extension)
        std::transform(s.begin(), s.end(), s.begin(), tolower);

        static const sFormatExt format[] =
        {
            { ".jpeg", TYPE_JPG },
            { ".jpg",  TYPE_JPG },
            { ".psd",  TYPE_PSD },
            { ".png",  TYPE_PNG },
            { ".gif",  TYPE_GIF },
            { ".ico",  TYPE_ICO },
            { ".tiff", TYPE_TIF },
            { ".tif",  TYPE_TIF },
            { ".xwd",  TYPE_XWD },
            { ".dds",  TYPE_DDS },
            { ".ppm",  TYPE_PPM },
        };

        for(size_t i = 0; i < sizeof(format) / sizeof(format[0]); i++)
        {
            if(s.substr(pos) == format[i].ext)
            {
                return format[i].format;
            }
        }

#if defined(IMLIB2_SUPPORT)
        return TYPE_COMMON;
#endif
    }

    return TYPES_COUNT;
}

