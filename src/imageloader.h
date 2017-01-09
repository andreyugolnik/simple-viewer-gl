/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "formats/format.h"
#include "common/bitmap_description.h"

#include <memory>
#include <thread>

class iCallbacks;

enum class eImageType
{
#if defined(IMLIB2_SUPPORT)
    COMMON,
#endif
    JPG,
    PSD,
    PNG,
    GIF,
    ICO,
    TIF,
    XWD,
    DDS,
    RAW,
    AGE,
    PPM,
    PVR,
    SCR,
    WEBP,

    NOTAVAILABLE,

    COUNT
};

class CImageLoader final
{
public:
    CImageLoader(iCallbacks* callbacks);
    ~CImageLoader();

    void LoadImage(const char* path);
    void LoadSubImage(unsigned subImage);
    bool isLoaded() const;

    const unsigned char* GetBitmap() const;
    unsigned GetWidth() const;
    unsigned GetHeight() const;
    unsigned GetPitch() const;
    unsigned GetBitmapFormat() const;
    unsigned GetBpp() const;
    unsigned GetImageBpp() const;
    long GetFileSize() const;
    size_t GetSizeMem() const;
    unsigned getCurrent() const;
    unsigned getImages() const;
    const char* getImageType() const;

private:
    void stop();
    void clear();
    eImageType getType(const char* name);
    void Load(const char* path);

private:
    iCallbacks* m_callbacks;

    std::thread m_loader;
    CFormat* m_image = nullptr;
    std::unique_ptr<CFormat> m_formats[(unsigned)eImageType::COUNT];
    sBitmapDescription m_desc;
};
