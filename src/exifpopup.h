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
    void createFont();

private:
    float m_ratio = 1.0f;
    float m_width = 0.0f;
    float m_height = 0.0f;
    std::string m_exif;
    std::unique_ptr<cQuad> m_bg;
    std::unique_ptr<cFTString> m_ft;
};
