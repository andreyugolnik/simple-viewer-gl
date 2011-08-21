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
#include <iostream>
#include <algorithm>

CImageLoader::CImageLoader(Callback callback)
    : m_angle(ANGLE_0)
    , m_callback(callback)
{
}

CImageLoader::~CImageLoader()
{
}

bool CImageLoader::LoadImage(const char* path, int subImage)
{
    if(path != 0)
    {
        if(m_path.empty() == false && m_path == path)
        {
            if(m_image.get() && !m_image->m_bitmap.empty() && GetSub() == subImage)
            {
                return true;	// image already loaded
            }
        }

        m_angle = ANGLE_0;
        m_path = path;

        int format = getFormat();
        switch(format)
        {
        case FORMAT_JPEG:
            m_image.reset(new CFormatJpeg(m_callback));
            break;
        case FORMAT_PSD:
            m_image.reset(new CFormatPsd(m_callback));
            break;
        case FORMAT_PNG:
            m_image.reset(new CFormatPng(m_callback));
            break;
        case FORMAT_GIF:
            if(!subImage)
            {
                m_image.reset(new CFormatGif(m_callback));
            }
            break;
        case FORMAT_ICO:
            m_image.reset(new CFormatIco(m_callback));
            break;
        case FORMAT_TIFF:
            m_image.reset(new CFormatTiff(m_callback));
            break;
        case FORMAT_XWD:
            m_image.reset(new CFormatXwd(m_callback));
            break;
        case FORMAT_DDS:
            m_image.reset(new CFormatDds(m_callback));
            break;
        default: //FORMAT_COMMON:
            m_image.reset(new CFormatCommon(m_callback));
            break;
        }

        return m_image->Load(path, subImage);
    }

    return false;
}

unsigned char* CImageLoader::GetBitmap() const
{
    if(m_image.get() && !m_image->m_bitmap.empty())
    {
        return (unsigned char*)&m_image->m_bitmap[0];
    }
    return 0;
}

void CImageLoader::FreeMemory()
{
    if(m_image.get() != 0)
    {
        m_image->FreeMemory();
        m_path.clear();
    }
}

int CImageLoader::GetWidth() const
{
    if(m_image.get() != 0)
    {
        if(m_angle == ANGLE_90 || m_angle == ANGLE_270)
        {
            return m_image->m_height;
        }
        return m_image->m_width;
    }
    return 0;
}

int CImageLoader::GetHeight() const
{
    if(m_image.get() != 0)
    {
        if(m_angle == ANGLE_90 || m_angle == ANGLE_270)
        {
            return m_image->m_width;
        }
        return m_image->m_height;
    }
    return 0;
}

int CImageLoader::GetPitch() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_pitch;
    }
    return 0;
}

int CImageLoader::GetBitmapFormat() const
{
    if(m_image.get() != 0) {
        return m_image->m_format;
    }
    return 0;
}

int CImageLoader::GetBpp() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_bpp;
    }
    return 0;
}

int CImageLoader::GetImageBpp() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_bppImage;
    }
    return 0;
}

// file size on disk
long CImageLoader::GetSize() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_size;
    }
    return 0;
}

size_t CImageLoader::GetSizeMem() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_bitmap.size();
    }
    return 0;
}

int CImageLoader::GetSub() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_subImage;
    }
    return 0;
}

int CImageLoader::GetSubCount() const
{
    if(m_image.get() != 0)
    {
        return m_image->m_subCount;
    }
    return 0;
}

int CImageLoader::getFormat()
{
    std::string s(m_path);

    // skip file without extension
    size_t pos	= s.find_last_of('.');
    if(std::string::npos == pos)
    {
        return false;
    }

    // skip non image file (detect by extension)
    std::transform(s.begin(), s.end(), s.begin(), tolower);

    FORMAT format[]	= {
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

