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

    const int Border = 4.0f;
    const int AlphaColor = 200;
    const int DesiredFontSize = 13;

}

void cExifPopup::init()
{
    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor({ 0, 0, 0, AlphaColor });

    createFont();
}

void cExifPopup::setRatio(float ratio)
{
    if (m_ratio != ratio)
    {
        m_ratio = ratio;
        createFont();
    }
}

void cExifPopup::createFont()
{
    m_ft.reset(new cFTString(DesiredFontSize * m_ratio));
    m_ft->setColor({ 255, 255, 255, AlphaColor });
}

void cExifPopup::setExifList(const sBitmapDescription::ExifList& exifList)
{
    m_exif.clear();

    auto rows = exifList.size();
    if (rows != 0)
    {
        float width = 0.0f;
        for (const auto& e : exifList)
        {
            std::string tag;

            tag += e.tag;
            tag += ": ";
            tag += e.value;
            width = std::max<float>(width, m_ft->getStringWidth(tag.c_str()));

            m_exif += tag;
            m_exif += "\n";
        }

        m_width = width + 2.0f * Border * m_ratio;
        m_height = (DesiredFontSize * rows + 2.0f * Border) * m_ratio;
    }
}

void cExifPopup::render()
{
    if (m_exif.empty() == false)
    {
        const float x = 5.0f;
        const float y = 5.0f;

        m_bg->SetSpriteSize(m_width, m_height);
        m_bg->Render(x, y);
        m_ft->draw(x + Border * m_ratio, y + DesiredFontSize * m_ratio, m_exif.c_str());
    }
}
