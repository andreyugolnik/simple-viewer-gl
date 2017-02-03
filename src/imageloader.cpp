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
#include "common/helpers.h"
#include "formats/formatage.h"
#include "formats/formatbmp.h"
#include "formats/formatcommon.h"
#include "formats/formatdds.h"
#include "formats/formatgif.h"
#include "formats/formatico.h"
#include "formats/formatjpeg.h"
#include "formats/formatpng.h"
#include "formats/formatpnm.h"
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

cImageLoader::cImageLoader(iCallbacks* callbacks)
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
    m_formats[(unsigned)eImageType::PNM].reset(new cFormatPnm(nullptr, callbacks));
    m_formats[(unsigned)eImageType::PVR].reset(new cFormatPvr(nullptr, callbacks));
    m_formats[(unsigned)eImageType::SCR].reset(new cFormatScr(nullptr, callbacks));
    m_formats[(unsigned)eImageType::TGA].reset(new cFormatTarga(nullptr, callbacks));
    m_formats[(unsigned)eImageType::BMP].reset(new cFormatBmp(nullptr, callbacks));
    m_formats[(unsigned)eImageType::WEBP].reset(new cFormatWebP(nullptr, callbacks));

    m_formats[(unsigned)eImageType::NOTAVAILABLE].reset(new cNotAvailable());
}

cImageLoader::~cImageLoader()
{
    stop();
    clear();
}

void cImageLoader::Load(const char* path)
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

void cImageLoader::LoadImage(const char* path)
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

void cImageLoader::LoadSubImage(unsigned subImage)
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

bool cImageLoader::isLoaded() const
{
    return m_image != m_formats[(unsigned)eImageType::NOTAVAILABLE].get()
           && !m_desc.bitmap.empty();
}

const unsigned char* cImageLoader::GetBitmap() const
{
    if (!m_desc.bitmap.empty())
    {
        return &m_desc.bitmap[0];
    }
    return nullptr;
}

void cImageLoader::stop()
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

void cImageLoader::clear()
{
    m_desc.reset();
}

unsigned cImageLoader::GetWidth() const
{
    return m_desc.width;
}

unsigned cImageLoader::GetHeight() const
{
    return m_desc.height;
}

unsigned cImageLoader::GetPitch() const
{
    return m_desc.pitch;
}

unsigned cImageLoader::GetBitmapFormat() const
{
    return m_desc.format;
}

unsigned cImageLoader::GetBpp() const
{
    return m_desc.bpp;
}

unsigned cImageLoader::GetImageBpp() const
{
    return m_desc.bppImage;
}

long cImageLoader::GetFileSize() const
{
    return m_desc.size;
}

size_t cImageLoader::GetSizeMem() const
{
    return m_desc.bitmap.size();
}

unsigned cImageLoader::getCurrent() const
{
    return m_desc.current;
}

unsigned cImageLoader::getImages() const
{
    return m_desc.images;
}

const char* cImageLoader::getImageType() const
{
    if (m_image != nullptr)
    {
        return m_image->getFormatName();
    }
    return nullptr;
}

namespace
{

#define LOADER_NAME 0
#if LOADER_NAME == 1
    const char* typeToName(eImageType type)
    {
        const char* Names[] =
        {
#if defined(IMLIB2_SUPPORT)
            "COMMON",
#endif
            "JPG",
            "PSD",
            "PNG",
            "GIF",
            "ICO",
            "TIF",
            "XWD",
            "XPM",
            "DDS",
            "RAW",
            "AGE",
            "PNM",
            "PVR",
            "SCR",
            "TGA",
            "BMP",
            "WEBP",

            "NOTAVAILABLE",
        };

        const auto idx = (size_t)type;
        const auto size = helpers::countof(Names);
        assert(size == (size_t)eImageType::COUNT);
        return idx < size ? Names[idx] : "";
    };
#endif

}

eImageType cImageLoader::getType(const char* name)
{
    cFile file;
    if (file.open(name))
    {
        const eImageType SortedTypes[] =
        {
            eImageType::AGE, // may be any extension, not only .age
            eImageType::RAW, // or .raw

            eImageType::JPG,
            eImageType::PSD,
            eImageType::PNG,
            eImageType::GIF,
            eImageType::ICO,
            eImageType::TIF,
            eImageType::XWD,
            eImageType::XPM,
            eImageType::DDS,
            eImageType::PNM,
            eImageType::PVR,
            eImageType::BMP,
            eImageType::TGA,
            eImageType::WEBP,
            eImageType::SCR,

#if defined(IMLIB2_SUPPORT)
            eImageType::COMMON, // use it as fallback loader
#endif
        };

        Buffer buffer;
        for (auto type : SortedTypes)
        {
            if (m_formats[(size_t)type]->isSupported(file, buffer))
            {
#if LOADER_NAME == 1
                ::printf("(II) Loader by type %s\n", typeToName(type));
#endif
                return type;
            }
        }

        ::printf("(WW) Loader not available for '%s'.\n", name);
    }

    return eImageType::NOTAVAILABLE;
}
