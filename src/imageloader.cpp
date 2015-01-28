/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageloader.h"
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
#include <iostream>
#include <algorithm>

CImageLoader::CImageLoader(iCallbacks* callbacks)
    : m_callbacks(callbacks)
    , m_image(0)
{
#if defined(IMLIB2_SUPPORT)
    m_format_common.reset(new CFormatCommon("libImlib2", "ImLib2"));
#endif
    m_format_jpeg.reset(new CFormatJpeg("libjpeg", "JPEG"));
    m_format_psd.reset(new CFormatPsd(0, "PSD"));
    m_format_png.reset(new CFormatPng("libpng", "PNG"));
    m_format_gif.reset(new CFormatGif("libgif", "GIF"));
    m_format_ico.reset(new CFormatIco(0, "ICO"));
    m_format_tiff.reset(new CFormatTiff("libtiff", "TIFF"));
    m_format_xwd.reset(new CFormatXwd(0, "XWD"));
    m_format_dds.reset(new CFormatDds(0, "DDS"));
    m_format_raw.reset(new cFormatRaw(0, "RAW"));
    m_format_ppm.reset(new cFormatPpm(0, "PPM"));
}

CImageLoader::~CImageLoader()
{
}

bool CImageLoader::LoadImage(const char* path, unsigned subImage)
{
    if(path != 0)
    {
        if(m_path.empty() == false && m_path == path)
        {
            if(m_image && !m_image->m_bitmap.empty() && GetSub() == subImage)
            {
                return true; // image already loaded
            }
        }

        m_path = path;
        if(m_image)
        {
            m_image->reset();
        }

        const int format = getFormat();
        switch(format)
        {
        case FORMAT_JPEG:
            m_image = m_format_jpeg.get();
            break;
        case FORMAT_PSD:
            m_image = m_format_psd.get();
            break;
        case FORMAT_PNG:
            m_image = m_format_png.get();
            break;
        case FORMAT_GIF:
            if(!subImage)
            {
                m_image = m_format_gif.get();
            }
            break;
        case FORMAT_ICO:
            m_image = m_format_ico.get();
            break;
        case FORMAT_TIFF:
            m_image = m_format_tiff.get();
            break;
        case FORMAT_XWD:
            m_image = m_format_xwd.get();
            break;
        case FORMAT_DDS:
            m_image = m_format_dds.get();
            break;
        case FORMAT_RAW:
            m_image = m_format_raw.get();
            break;
        case FORMAT_PPM:
            m_image = m_format_ppm.get();
            break;
        default: //FORMAT_COMMON:
#if defined(IMLIB2_SUPPORT)
            m_image = m_format_common.get();
#else
            return false;
#endif
            break;
        }

        m_image->setCallbacks(m_callbacks);
        const bool result = m_image->Load(path, subImage);
        if(!result)
        {
            m_image->reset();
        }
        return result;
    }

    return false;
}

unsigned char* CImageLoader::GetBitmap() const
{
    if(m_image && !m_image->m_bitmap.empty())
    {
        return (unsigned char*)&m_image->m_bitmap[0];
    }
    return 0;
}

void CImageLoader::FreeMemory()
{
    if(m_image != 0)
    {
        m_image->FreeMemory();
        m_path.clear();
    }
}

unsigned CImageLoader::GetWidth() const
{
    if(m_image != 0)
    {
        return m_image->m_width;
    }
    return 0;
}

unsigned CImageLoader::GetHeight() const
{
    if(m_image != 0)
    {
        return m_image->m_height;
    }
    return 0;
}

unsigned CImageLoader::GetPitch() const
{
    if(m_image != 0)
    {
        return m_image->m_pitch;
    }
    return 0;
}

unsigned CImageLoader::GetBitmapFormat() const
{
    if(m_image != 0) {
        return m_image->m_format;
    }
    return 0;
}

unsigned CImageLoader::GetBpp() const
{
    if(m_image != 0)
    {
        return m_image->m_bpp;
    }
    return 0;
}

unsigned CImageLoader::GetImageBpp() const
{
    if(m_image != 0)
    {
        return m_image->m_bppImage;
    }
    return 0;
}

// file size on disk
long CImageLoader::GetSize() const
{
    if(m_image != 0)
    {
        return m_image->m_size;
    }
    return 0;
}

size_t CImageLoader::GetSizeMem() const
{
    if(m_image != 0)
    {
        return m_image->m_bitmap.size();
    }
    return 0;
}

unsigned CImageLoader::GetSub() const
{
    if(m_image != 0)
    {
        return m_image->m_subImage;
    }
    return 0;
}

unsigned CImageLoader::GetSubCount() const
{
    if(m_image != 0)
    {
        return m_image->m_subCount;
    }
    return 0;
}

unsigned CImageLoader::getFormat()
{
    if(m_format_raw->isRawFormat(m_path.c_str()))
    {
        return FORMAT_RAW;
    }

    std::string s(m_path);

    // skip file without extension
    size_t pos = s.find_last_of('.');
    if(std::string::npos == pos)
    {
        return false;
    }

    // skip non image file (detect by extension)
    std::transform(s.begin(), s.end(), s.begin(), tolower);

    FORMAT format[] = {
        { ".jpeg", FORMAT_JPEG },
        { ".jpg",  FORMAT_JPEG },
        { ".psd",  FORMAT_PSD  },
        { ".png",  FORMAT_PNG  },
        { ".gif",  FORMAT_GIF  },
        { ".ico",  FORMAT_ICO  },
        { ".tiff", FORMAT_TIFF },
        { ".tif",  FORMAT_TIFF },
        { ".xwd",  FORMAT_XWD  },
        { ".dds",  FORMAT_DDS  },
        { ".raw",  FORMAT_RAW  },
        { ".ppm",  FORMAT_PPM  },
    };

    for(size_t i = 0; i < sizeof(format) / sizeof(FORMAT); i++)
    {
        if(s.substr(pos) == format[i].ext)
        {
            return format[i].format;
        }
    }

    return FORMAT_COMMON;
}

