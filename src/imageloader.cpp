/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "imageloader.h"
#include "common/bitmap_description.h"
#include "common/callbacks.h"
#include "common/file.h"
#include "formats/formatage.h"
#include "formats/formatcommon.h"
#include "formats/formatdds.h"
#include "formats/formatgif.h"
#include "formats/formatico.h"
#include "formats/formatjpeg.h"
#include "formats/formatpng.h"
#include "formats/formatppm.h"
#include "formats/formatpsd.h"
#include "formats/formatpvr.h"
#include "formats/formatraw.h"
#include "formats/formatscr.h"
#include "formats/formattiff.h"
#include "formats/formatxwd.h"
#include "notavailable.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

CImageLoader::CImageLoader(iCallbacks* callbacks)
    : m_callbacks(callbacks)
{
#if defined(IMLIB2_SUPPORT)
    m_formats[(unsigned)eImageType::COMMON].reset(new CFormatCommon("libImlib2", "ImLib2", callbacks));
#endif
    m_formats[(unsigned)eImageType::JPG].reset(new CFormatJpeg("libjpeg", "jpeg", callbacks));
    m_formats[(unsigned)eImageType::PSD].reset(new CFormatPsd(nullptr, "psd", callbacks));
    m_formats[(unsigned)eImageType::PNG].reset(new CFormatPng("libpng", "png", callbacks));
    m_formats[(unsigned)eImageType::GIF].reset(new CFormatGif("libgif", "gif", callbacks));
    m_formats[(unsigned)eImageType::ICO].reset(new CFormatIco(nullptr, "ico", callbacks));
    m_formats[(unsigned)eImageType::TIF].reset(new CFormatTiff("libtiff", "tiff", callbacks));
    m_formats[(unsigned)eImageType::XWD].reset(new CFormatXwd(nullptr, "xwd", callbacks));
    m_formats[(unsigned)eImageType::DDS].reset(new CFormatDds(nullptr, "dds", callbacks));
    m_formats[(unsigned)eImageType::RAW].reset(new cFormatRaw(nullptr, "raw", callbacks));
    m_formats[(unsigned)eImageType::AGE].reset(new cFormatAge(nullptr, "age", callbacks));
    m_formats[(unsigned)eImageType::PPM].reset(new cFormatPpm(nullptr, "ppm", callbacks));
    m_formats[(unsigned)eImageType::PVR].reset(new cFormatPvr(nullptr, "pvr", callbacks));
    m_formats[(unsigned)eImageType::SCR].reset(new cFormatScr(nullptr, "scr", callbacks));

    m_formats[(unsigned)eImageType::NOTAVAILABLE].reset(new CNotAvailable());
}

CImageLoader::~CImageLoader()
{
    stop();
    clear();
}

void CImageLoader::Load(const char* path)
{
    if (path != nullptr)
    {
        const eImageType type = getType(path);
        m_image = m_formats[(unsigned)type].get();
        if (m_image->Load(path, m_desc))
        {
            return;
        }
    }

    m_image = m_formats[(unsigned)eImageType::NOTAVAILABLE].get();
    m_image->Load(path, m_desc);
}

void CImageLoader::LoadImage(const char* path)
{
    stop();
    clear();

    m_loader = std::thread([this](const char* path)
    {
        m_callbacks->startLoading();
        Load(path);
        if (m_desc.images == 0)
        {
            m_desc.images = 1;
        }
        m_callbacks->endLoading();
    }, path);
}

void CImageLoader::LoadSubImage(unsigned subImage)
{
    assert(m_image != nullptr);

    stop();

    m_loader = std::thread([this](unsigned subImage)
    {
        m_callbacks->startLoading();
        m_image->LoadSubImage(subImage, m_desc);
        m_callbacks->endLoading();
    }, subImage);
}

bool CImageLoader::isLoaded() const
{
    return m_image != m_formats[(unsigned)eImageType::NOTAVAILABLE].get()
        && !m_desc.bitmap.empty();
}

const unsigned char* CImageLoader::GetBitmap() const
{
    if (!m_desc.bitmap.empty())
    {
        return &m_desc.bitmap[0];
    }
    return nullptr;
}

void CImageLoader::stop()
{
    if (m_loader.joinable())
    {
        if (m_image != nullptr)
        {
            m_image->stop();
        }
        m_loader.join();
    }
}

void CImageLoader::clear()
{
    m_desc.bitmap.clear();
    m_desc.format   = GL_RGB;
    m_desc.bpp      = 0;
    m_desc.pitch    = 0;
    m_desc.width    = 0;
    m_desc.height   = 0;
    m_desc.bppImage = 0;
    m_desc.size     = -1;
    m_desc.images   = 0;
    m_desc.current  = 0;
    m_desc.info.clear();
}

unsigned CImageLoader::GetWidth() const
{
    return m_desc.width;
}

unsigned CImageLoader::GetHeight() const
{
    return m_desc.height;
}

unsigned CImageLoader::GetPitch() const
{
    return m_desc.pitch;
}

unsigned CImageLoader::GetBitmapFormat() const
{
    return m_desc.format;
}

unsigned CImageLoader::GetBpp() const
{
    return m_desc.bpp;
}

unsigned CImageLoader::GetImageBpp() const
{
    return m_desc.bppImage;
}

long CImageLoader::GetFileSize() const
{
    return m_desc.size;
}

size_t CImageLoader::GetSizeMem() const
{
    return m_desc.bitmap.size();
}

unsigned CImageLoader::getCurrent() const
{
    return m_desc.current;
}

unsigned CImageLoader::getImages() const
{
    return m_desc.images;
}

const char* CImageLoader::getImageType() const
{
    if (m_image != nullptr)
    {
        return m_image->getFormatName();
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
    if (!file.open(name))
    {
        return eImageType::NOTAVAILABLE;
    }

    Buffer buffer;
    if (m_formats[(unsigned)eImageType::AGE]->isSupported(file, buffer))
    {
        return eImageType::AGE;
    }
    if (m_formats[(unsigned)eImageType::RAW]->isSupported(file, buffer))
    {
        return eImageType::RAW;
    }
    if (m_formats[(unsigned)eImageType::PVR]->isSupported(file, buffer))
    {
        return eImageType::PVR;
    }

    std::string s(name);

    // skip file without extension
    const size_t pos = s.find_last_of('.');
    if (std::string::npos != pos)
    {
        // skip non image file (detect by extension)
        std::transform(s.begin(), s.end(), s.begin(), tolower);

        static const sFormatExt format[] =
        {
            { ".jpeg", eImageType::JPG },
            { ".jpg",  eImageType::JPG },
            { ".psd",  eImageType::PSD },
            { ".png",  eImageType::PNG },
            { ".gif",  eImageType::GIF },
            { ".ico",  eImageType::ICO },
            { ".tiff", eImageType::TIF },
            { ".tif",  eImageType::TIF },
            { ".xwd",  eImageType::XWD },
            { ".dds",  eImageType::DDS },
            { ".ppm",  eImageType::PPM },
            { ".scr",  eImageType::SCR },
        };

        for (auto& fmt : format)
        {
            if (s.substr(pos) == fmt.ext)
            {
                return fmt.format;
            }
        }

#if defined(IMLIB2_SUPPORT)
        return eImageType::COMMON;
#endif
    }

    return eImageType::NOTAVAILABLE;
}
