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
#include "formats/formattarga.h"
#include "formats/formattiff.h"
#include "formats/formatwebp.h"
#include "formats/formatxpm.h"
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
    m_formats[(unsigned)eImageType::COMMON].reset(new cFormatCommon("libImlib2", callbacks));
#endif
    m_formats[(unsigned)eImageType::JPG].reset(new cFormatJpeg("libjpeg", callbacks));
    m_formats[(unsigned)eImageType::PSD].reset(new cFormatPsd(nullptr, callbacks));
    m_formats[(unsigned)eImageType::PNG].reset(new cFormatPng("libpng", callbacks));
    m_formats[(unsigned)eImageType::GIF].reset(new cFormatGif("libgif", callbacks));
    m_formats[(unsigned)eImageType::ICO].reset(new cFormatIco(nullptr, callbacks));
    m_formats[(unsigned)eImageType::TIF].reset(new cFormatTiff("libtiff", callbacks));
    m_formats[(unsigned)eImageType::XWD].reset(new cFormatXwd(nullptr, callbacks));
    m_formats[(unsigned)eImageType::XPM].reset(new cFormatXpm(nullptr, callbacks));
    m_formats[(unsigned)eImageType::DDS].reset(new cFormatDds(nullptr, callbacks));
    m_formats[(unsigned)eImageType::RAW].reset(new cFormatRaw(nullptr, callbacks));
    m_formats[(unsigned)eImageType::AGE].reset(new cFormatAge(nullptr, callbacks));
    m_formats[(unsigned)eImageType::PPM].reset(new cFormatPpm(nullptr, callbacks));
    m_formats[(unsigned)eImageType::PVR].reset(new cFormatPvr(nullptr, callbacks));
    m_formats[(unsigned)eImageType::SCR].reset(new cFormatScr(nullptr, callbacks));
    m_formats[(unsigned)eImageType::TGA].reset(new cFormatTarga(nullptr, callbacks));
    m_formats[(unsigned)eImageType::WEBP].reset(new cFormatWebP(nullptr, callbacks));

    m_formats[(unsigned)eImageType::NOTAVAILABLE].reset(new cNotAvailable());
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

    m_mode = Mode::Image;
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

    m_mode = Mode::SubImage;
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
    m_desc.reset();
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
    for (unsigned idx = 0; idx < (unsigned)eImageType::COUNT; idx++)
    {
        if (m_formats[idx]->isSupported(file, buffer))
        {
            return (eImageType)idx;
        }
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
            { ".jpeg",  eImageType::JPG  },
            { ".jpg",   eImageType::JPG  },
            { ".psd",   eImageType::PSD  },
            { ".png",   eImageType::PNG  },
            { ".gif",   eImageType::GIF  },
            { ".ico",   eImageType::ICO  },
            { ".tiff",  eImageType::TIF  },
            { ".tif",   eImageType::TIF  },
            { ".xwd",   eImageType::XWD  },
            { ".xpm",   eImageType::XWD  },
            { ".dds",   eImageType::DDS  },
            { ".ppm",   eImageType::PPM  },
            { ".scr",   eImageType::SCR  },
            { ".atr",   eImageType::SCR  },
            { ".ifl",   eImageType::SCR  },
            { ".bsc",   eImageType::SCR  },
            { ".bmc4",  eImageType::SCR  },
            { ".mc",    eImageType::SCR  },
            { ".tga",   eImageType::TGA  },
            { ".targa", eImageType::TGA  },
            { ".webp",  eImageType::WEBP },
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
