/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "formats/format.h"

class iCallbacks;
class CFormatCommon;
class CFormatDds;
class CFormatGif;
class CFormatIco;
class CFormatJpeg;
class CFormatPng;
class CFormatPsd;
class CFormatTiff;
class CFormatXwd;
class cFormatPpm;
class cFormatRaw;

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
    TYPE_PPM,

    TYPES_COUNT
};

class CImageLoader
{
public:
    CImageLoader(iCallbacks* callbacks);
    virtual ~CImageLoader();

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
    long GetSize() const;
    size_t GetSizeMem() const;
    unsigned GetSub() const;
    unsigned GetSubCount() const;
    const char* getImageType() const;

private:
    CImageLoader();
    eImageType getType(const char* name);

private:
    iCallbacks* m_callbacks;
    CFormat* m_image;
    eImageType m_type;
    CFormat* m_formats[TYPES_COUNT];
};

#endif // IMAGELOADER_H

