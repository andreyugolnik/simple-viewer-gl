/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "formats/format.h"

#include <memory>
#include <string>

class CFormatCommon;
class CFormatJpeg;
class CFormatPsd;
class CFormatPng;
class CFormatGif;
class CFormatIco;
class CFormatTiff;
class CFormatXwd;
class CFormatDds;

class CImageLoader
{
public:
    CImageLoader(Callback _callback);
    virtual ~CImageLoader();

    bool LoadImage(const char* path, int subImage);

    unsigned char* GetBitmap() const;
    void FreeMemory();
    int GetWidth() const;
    int GetHeight() const;
    int GetPitch() const;
    int GetBitmapFormat() const;
    int GetBpp() const;
    int GetImageBpp() const;
    long GetSize() const;
    size_t GetSizeMem() const;
    int GetSub() const;
    int GetSubCount() const;

private:
    CFormat* m_image;
    std::auto_ptr<CFormatCommon> m_format_common;
    std::auto_ptr<CFormatJpeg> m_format_jpeg;
    std::auto_ptr<CFormatPsd> m_format_psd;
    std::auto_ptr<CFormatPng> m_format_png;
    std::auto_ptr<CFormatGif> m_format_gif;
    std::auto_ptr<CFormatIco> m_format_ico;
    std::auto_ptr<CFormatTiff> m_format_tiff;
    std::auto_ptr<CFormatXwd> m_format_xwd;
    std::auto_ptr<CFormatDds> m_format_dds;
    std::string m_path;

private:
    CImageLoader();

    enum FILE_FORMAT { FORMAT_JPEG, FORMAT_PSD, FORMAT_PNG, FORMAT_GIF, FORMAT_ICO, FORMAT_TIFF, FORMAT_XWD, FORMAT_DDS, FORMAT_COMMON };
    struct FORMAT
    {
        const char* ext;
        int format;
    };
    int getFormat();
};

#endif // IMAGELOADER_H

