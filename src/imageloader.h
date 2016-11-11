/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#pragma once

#include "formats/format.h"

#include <memory>
#include <string>

class iCallbacks;

enum eImageType
{
#if defined(IMLIB2_SUPPORT)
    TYPE_COMMON,
#endif
    TYPE_JPG,
    TYPE_PSD,
    TYPE_PNG,
    TYPE_GIF,
    TYPE_ICO,
    TYPE_TIF,
    TYPE_XWD,
    TYPE_DDS,
    TYPE_RAW,
    TYPE_AGE,
    TYPE_PPM,
    TYPE_PVR,
    TYPE_SCR,

    TYPE_NOTAVAILABLE,

    TYPES_COUNT
};

class CImageLoader final
{
public:
    CImageLoader(iCallbacks* callbacks);
    ~CImageLoader();

    bool LoadImage(const char* path, unsigned subImage);
    bool isLoaded() const;

    unsigned char* GetBitmap() const;
    void FreeMemory();
    unsigned GetWidth() const;
    unsigned GetHeight() const;
    unsigned GetPitch() const;
    unsigned GetBitmapFormat() const;
    unsigned GetBpp() const;
    unsigned GetImageBpp() const;
    long GetFileSize() const;
    size_t GetSizeMem() const;
    unsigned GetSub() const;
    unsigned GetSubCount() const;
    const char* getImageType() const;

private:
    CImageLoader();
    eImageType getType(const char* name);
    bool LoadImage(const char* path, eImageType type, unsigned subImage);

private:
    CFormat* m_image;
    eImageType m_type;
    std::string m_path;
    std::unique_ptr<CFormat> m_formats[TYPES_COUNT];
};
