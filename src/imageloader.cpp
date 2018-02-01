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
#include "formats/formateps.h"
#include "formats/formatexr.h"
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
#include <cstdio>
#include <string>

cImageLoader::cImageLoader(iCallbacks* callbacks)
    : m_callbacks(callbacks)
{
    for (uint32_t i = 0; i < (uint32_t)eImageType::COUNT; i++)
    {
        createLoader((eImageType)i);
    }
}

cImageLoader::~cImageLoader()
{
    stop();
    clear();
}

void cImageLoader::load(const char* path)
{
    if (path != nullptr)
    {
        const eImageType type = getType(path);
        m_image = getLoader(type);

        // auto start = helpers::getTime();
        const bool result = m_image->Load(path, m_desc);
        // ::printf("(II) Loading time: %u μs.\n", (uint32_t)(helpers::getTime() - start) / 1000);

        if (result == true)
        {
            return;
        }
    }

    m_image = getLoader(eImageType::NOTAVAILABLE);
    m_image->Load(path, m_desc);
}

void cImageLoader::loadImage(const char* path)
{
    stop();
    clear();

    m_mode = Mode::Image;
    m_loader = std::thread([this](const char* path)
    {
        m_callbacks->startLoading();
        load(path);
        if (m_desc.images == 0)
        {
            m_desc.images = 1;
        }
        m_callbacks->endLoading();
    }, path);
}

void cImageLoader::loadSubImage(unsigned subImage)
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
    auto notAvailable = getLoader(eImageType::NOTAVAILABLE);
    return m_image != notAvailable && m_desc.bitmap.empty() == false;
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

// #define LOADER_NAME
#if defined(LOADER_NAME)
    const char* typeToName(eImageType type)
    {
        const char* Names[] =
        {
#if defined(IMLIB2_SUPPORT)
            "COMMON",
#endif
#if defined(OPENEXR_SUPPORT)
            "EXR",
#endif
            "JPG",
            "PSD",
            "EPS",
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
    }
#endif

}

cFormat* cImageLoader::createLoader(eImageType type)
{
    auto& format = m_formats[(unsigned)type];
    assert(m_formats[(unsigned)type].get() == nullptr);

    cFormat* loader = nullptr;

    switch (type)
    {
#if defined(IMLIB2_SUPPORT)
    case eImageType::COMMON:
        loader = new cFormatCommon(m_callbacks);
        break;
#endif

#if defined(OPENEXR_SUPPORT)
    case eImageType::EXR:
        loader = new cFormatExr(m_callbacks);
        break;
#endif

    case eImageType::JPG:
        loader = new cFormatJpeg(m_callbacks);
        break;

    case eImageType::PSD:
        loader = new cFormatPsd(m_callbacks);
        break;

    case eImageType::EPS:
        loader = new cFormatEps(m_callbacks);
        break;

    case eImageType::PNG:
        loader = new cFormatPng(m_callbacks);
        break;

    case eImageType::GIF:
        loader = new cFormatGif(m_callbacks);
        break;

    case eImageType::ICO:
        loader = new cFormatIco(m_callbacks);
        break;

    case eImageType::TIF:
        loader = new cFormatTiff(m_callbacks);
        break;

    case eImageType::XWD:
        loader = new cFormatXwd(m_callbacks);
        break;

    case eImageType::XPM:
        loader = new cFormatXpm(m_callbacks);
        break;

    case eImageType::DDS:
        loader = new cFormatDds(m_callbacks);
        break;

    case eImageType::RAW:
        loader = new cFormatRaw(m_callbacks);
        break;

    case eImageType::AGE:
        loader = new cFormatAge(m_callbacks);
        break;

    case eImageType::PNM:
        loader = new cFormatPnm(m_callbacks);
        break;

    case eImageType::PVR:
        loader = new cFormatPvr(m_callbacks);
        break;

    case eImageType::SCR:
        loader = new cFormatScr(m_callbacks);
        break;

    case eImageType::TGA:
        loader = new cFormatTarga(m_callbacks);
        break;

    case eImageType::BMP:
        loader = new cFormatBmp(m_callbacks);
        break;

    case eImageType::WEBP:
        loader = new cFormatWebP(m_callbacks);
        break;

    case eImageType::NOTAVAILABLE:
        loader = new cNotAvailable();
        break;

    case eImageType::COUNT: // do nothing
        break;
    }

    format.reset(loader);

    return loader;
}

cFormat* cImageLoader::getLoader(eImageType type) const
{
    return m_formats[(unsigned)type].get();
}

eImageType cImageLoader::getType(const char* name)
{
    cFile file;
    if (file.open(name))
    {
        const eImageType SortedTypes[] =
        {
            eImageType::AGE,
            eImageType::RAW,
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
            eImageType::TGA,
            eImageType::WEBP,
            eImageType::BMP,
            eImageType::SCR,
            eImageType::EPS,

#if defined(OPENEXR_SUPPORT)
            eImageType::EXR,
#endif

#if defined(IMLIB2_SUPPORT)
            eImageType::COMMON, // use it as fallback loader
#endif
        };

        Buffer buffer;
        for (auto type : SortedTypes)
        {
            auto loader = getLoader(type);
            if (loader->isSupported(file, buffer))
            {
#if defined(LOADER_NAME)
                ::printf("(II) Loader by type %s\n", typeToName(type));
#endif
                return type;
            }
        }

        ::printf("(WW) Loader not available for '%s'.\n", name);
    }

    return eImageType::NOTAVAILABLE;
}
