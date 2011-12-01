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

class CImageLoader
{
public:
    CImageLoader(Callback callback);
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
    Callback m_callback;
    std::string m_path;
    std::auto_ptr<CFormat> m_image;

private:
    enum FILE_FORMAT { FORMAT_JPEG, FORMAT_PSD, FORMAT_PNG, FORMAT_GIF, FORMAT_ICO, FORMAT_TIFF, FORMAT_XWD, FORMAT_DDS, FORMAT_COMMON };
    struct FORMAT
    {
        const char* ext;
        int format;
    };
    int getFormat();
};

#endif // IMAGELOADER_H

