/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "ftstring.h"
#include "common/bitmap_description.h"

#include <memory>

class cExifPopup final
{
public:
    void init();
    void setRatio(float ratio);

    void setExifList(const sBitmapDescription::ExifList& exifList);
    void render();

private:
    void createFont(int fontSize);

private:
    float m_ratio = 1.0f;
    struct Exif
    {
        Vectorf tagOffset;
        std::string tag;
        Vectorf valueOffset;
        std::string value;
    };
    std::vector<Exif> m_exif;
    Vectorf m_bgSize;
    std::unique_ptr<cQuad> m_bg;
    std::unique_ptr<cFTString> m_ft;
};
