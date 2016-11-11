/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageloader.h"
#include "notavailable.h"
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
#include "formats/formatage.h"
#include "formats/formatraw.h"
#include "formats/formatppm.h"
#include "formats/formatpvr.h"
#include "formats/formatscr.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

CImageLoader::CImageLoader(iCallbacks* callbacks)
    : m_image(nullptr)
    , m_type(TYPES_COUNT)
{
#if defined(IMLIB2_SUPPORT)
    m_formats[TYPE_COMMON].reset(new CFormatCommon("libImlib2", "ImLib2", callbacks));
#endif
    m_formats[TYPE_JPG].reset(new CFormatJpeg("libjpeg", "jpeg", callbacks));
    m_formats[TYPE_PSD].reset(new CFormatPsd(nullptr, "psd", callbacks));
    m_formats[TYPE_PNG].reset(new CFormatPng("libpng", "png", callbacks));
    m_formats[TYPE_GIF].reset(new CFormatGif("libgif", "gif", callbacks));
    m_formats[TYPE_ICO].reset(new CFormatIco(nullptr, "ico", callbacks));
    m_formats[TYPE_TIF].reset(new CFormatTiff("libtiff", "tiff", callbacks));
    m_formats[TYPE_XWD].reset(new CFormatXwd(nullptr, "xwd", callbacks));
    m_formats[TYPE_DDS].reset(new CFormatDds(nullptr, "dds", callbacks));
    m_formats[TYPE_RAW].reset(new cFormatRaw(nullptr, "raw", callbacks));
    m_formats[TYPE_AGE].reset(new cFormatAge(nullptr, "age", callbacks));
    m_formats[TYPE_PPM].reset(new cFormatPpm(nullptr, "ppm", callbacks));
    m_formats[TYPE_PVR].reset(new cFormatPvr(nullptr, "pvr", callbacks));
    m_formats[TYPE_SCR].reset(new cFormatScr(nullptr, "scr", callbacks));

    m_formats[TYPE_NOTAVAILABLE].reset(new CNotAvailable());
}

CImageLoader::~CImageLoader()
{
}

bool CImageLoader::LoadImage(const char* path, unsigned subImage)
{
    if(path != nullptr && m_path == path)
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

    if (LoadImage(path, type, subImage))
    {
        return true;
    }

    FreeMemory();
    LoadImage(path, TYPE_NOTAVAILABLE, subImage);

    return false;
}

bool CImageLoader::LoadImage(const char* path, eImageType type, unsigned subImage)
{
    m_path.clear();
    if (path != nullptr)
    {
        m_path = path;
    }

    m_type = type;

    m_image = m_formats[type].get();
    const bool result = m_image->Load(path, subImage);
    return result;
}

bool CImageLoader::isLoaded() const
{
    return m_image != nullptr && !m_image->m_bitmap.empty();
}

unsigned char* CImageLoader::GetBitmap() const
{
    assert(m_image != nullptr);
    if(!m_image->m_bitmap.empty())
    {
        return &m_image->m_bitmap[0];
    }
    return nullptr;
}

void CImageLoader::FreeMemory()
{
    if(m_image != nullptr)
    {
        m_image->FreeMemory();
        m_path.clear();
    }
}

unsigned CImageLoader::GetWidth() const
{
    assert(m_image != nullptr);
    return m_image->m_width;
}

unsigned CImageLoader::GetHeight() const
{
    assert(m_image != nullptr);
    return m_image->m_height;
}

unsigned CImageLoader::GetPitch() const
{
    assert(m_image != nullptr);
    return m_image->m_pitch;
}

unsigned CImageLoader::GetBitmapFormat() const
{
    assert(m_image != nullptr);
    return m_image->m_format;
}

unsigned CImageLoader::GetBpp() const
{
    assert(m_image != nullptr);
    return m_image->m_bpp;
}

unsigned CImageLoader::GetImageBpp() const
{
    assert(m_image != nullptr);
    return m_image->m_bppImage;
}

long CImageLoader::GetFileSize() const
{
    assert(m_image != nullptr);
    return m_image->m_size;
}

size_t CImageLoader::GetSizeMem() const
{
    assert(m_image != nullptr);
    return m_image->m_bitmap.size();
}

unsigned CImageLoader::GetSub() const
{
    assert(m_image != nullptr);
    return m_image->m_subImage;
}

unsigned CImageLoader::GetSubCount() const
{
    assert(m_image);
    if(m_image != nullptr)
    {
        return m_image->m_subCount;
    }
    return 0;
}

const char* CImageLoader::getImageType() const
{
    assert(m_image);
    if(m_image != nullptr)
    {
        return m_image->m_formatName.c_str();
    }
    return nullptr;
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
        return TYPE_NOTAVAILABLE;
    }

    Buffer buffer;
    if(m_formats[TYPE_AGE]->isSupported(file, buffer))
    {
        return TYPE_AGE;
    }
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
            { ".scr",  TYPE_SCR },
        };

        for(auto& fmt : format)
        {
            if(s.substr(pos) == fmt.ext)
            {
                return fmt.format;
            }
        }

#if defined(IMLIB2_SUPPORT)
        return TYPE_COMMON;
#endif
    }

    return TYPE_NOTAVAILABLE;
}

