/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "exifpopup.h"

#include <cstring>

namespace
{

    const int AlphaColor = 200;
    const float Border = 10.0f;
    const float RowHeight = 30.0f;

}

void cExifPopup::init()
{
    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor({ 0, 0, 0, AlphaColor });

    const int DesiredFontSize = 30;
    createFont(DesiredFontSize);
}

void cExifPopup::setScale(float scale)
{
    m_scale = scale;
}

void cExifPopup::createFont(int fontSize)
{
    m_ft.reset(new cFTString(fontSize));
    m_ft->setColor({ 255, 255, 255, AlphaColor });
}

void cExifPopup::setExifList(const sBitmapDescription::ExifList& exifList)
{
    m_exif.clear();

    auto rows = exifList.size();
    if (rows != 0)
    {
        const float scale = m_scale;
        float width = 0.0f;
        float row = 0.0f;
        const float space = m_ft->getBounds(" ").x;
        for (const auto& e : exifList)
        {
            auto tagBounds = m_ft->getBounds(e.tag.c_str());
            auto valueBounds = m_ft->getBounds(e.value.c_str());

            m_exif.push_back(
            {
                Vectorf{ 0.0f, row } * scale,
                e.tag,
                Vectorf{ tagBounds.x + space, row } * scale,
                e.value
            });

            row += RowHeight;
            width = std::max<float>(width, tagBounds.x + valueBounds.x);
        }

        m_bgSize =
        {
            width + space + 2.0f * Border,
            RowHeight* rows + 2.0f * Border
        };
        m_bgSize *= scale;
    }
}

void cExifPopup::render()
{
    if (m_exif.empty() == false)
    {
        Vectorf pos{ 5.0f, 5.0f };

        const float scale = m_scale;

        m_bg->setSpriteSize(m_bgSize);
        m_bg->render(pos);

        const cColor tagColor{ 255, 255, 150, AlphaColor };
        const cColor valueColor{ 255, 255, 255, AlphaColor };
        pos += Vectorf{ Border, Border } * scale;
        for (const auto& s : m_exif)
        {
            m_ft->setColor(tagColor);
            m_ft->draw(pos + s.tagOffset, s.tag.c_str(), scale);
            m_ft->setColor(valueColor);
            m_ft->draw(pos + s.valueOffset, s.value.c_str(), scale);
        };
    }
}
