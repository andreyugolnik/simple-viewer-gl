/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "popup.h"
#include "common/bitmap_description.h"

class cExifPopup final : public cPopup
{
public:
    void init() override;
    void render() override;

    void setExifList(const sBitmapDescription::ExifList& exifList);

private:
    struct Exif
    {
        Vectorf tagOffset;
        std::string tag;
        Vectorf valueOffset;
        std::string value;
    };
    std::vector<Exif> m_exif;
    Vectorf m_bgSize;
};
