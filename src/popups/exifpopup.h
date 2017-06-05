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
#include "types/vector.h"
#include "common/bitmap_description.h"

class cExifPopup final : public cPopup
{
public:
    void render() override;

    void setExifList(const sBitmapDescription::ExifList& exifList);

private:
    sBitmapDescription::ExifList m_exif;
};
