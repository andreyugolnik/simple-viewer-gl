/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "pixelpopup.h"
#include "img-pointer-cross.c"
#include "img-icons.c"
#include "types/math.h"

#include <cstring>

namespace
{

    const float InfoOffset = 10.0f;

    const float IconsWidth = 26.0f;
    const float IconsHeight = 26.0f;

    const float TextOffsetX = 40.0f;
    const float TextOffsetY = 22.0f;

    const int AlphaColor = 200;
    const cColor GrayColor{ 155, 155, 155, AlphaColor };
    const cColor WhiteColor{ 255, 255, 255, AlphaColor };
}

void cPixelPopup::init()
{
    m_pixelInfo.reset();

    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor({ 0, 0, 0, AlphaColor });

    m_pointer.reset(new cQuadSeries(imgPointerCross.width, imgPointerCross.height
                                    , imgPointerCross.pixel_data, imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA));
    m_pointer->setup(21, 21, 10);
    setCursor(0);

    m_icons.reset(new cQuadSeries(imgIcons.width, imgIcons.height
                                  , imgIcons.pixel_data, imgIcons.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA));
    m_icons->setup(IconsWidth, IconsHeight, 4);
    m_icons->setColor({ 155, 155, 155, AlphaColor });
}

void cPixelPopup::setRatio(float ratio)
{
    ratio = 1.0f;
    // if (m_ratio != ratio)
    {
        const float RowHeight = 40.0f;
        m_rowHeight = RowHeight * ratio;

        const float Border = 10.0f;
        m_border = Border * ratio;

        m_iconOffset =
        {
            (m_rowHeight - IconsWidth) * 0.5f,
            (m_rowHeight - IconsHeight) * 0.5f
        };

        const int desiredFontSize = 28;
        createFont(desiredFontSize * ratio);

        m_ratio = ratio;
    }
}

void cPixelPopup::createFont(int fontSize)
{
    m_ft.reset(new cFTString(fontSize));
}

void cPixelPopup::setPixelInfo(const sPixelInfo& pi)
{
    m_pixelInfo = pi;

    char buffer[100];
    m_info.clear();

    ::snprintf(buffer, sizeof(buffer), "%d x %d", (int)pi.point.x, (int)pi.point.y);
    m_info.push_back({ Info::Type::Position, true, buffer });

    {
        buffer[0] = 0;

        if (pi.bpp == 32)
        {
            ::snprintf(buffer, sizeof(buffer), "0x%.2x%.2x%.2x%.2x"
                       , pi.color.a, pi.color.r, pi.color.g, pi.color.b);
        }
        else if (pi.bpp == 24 || pi.bpp == 16)
        {
            ::snprintf(buffer, sizeof(buffer), "0x%.2x%.2x%.2x"
                       , pi.color.r, pi.color.g, pi.color.b);
        }
        else if (pi.bpp == 8)
        {
            ::snprintf(buffer, sizeof(buffer), "0x%.2x"
                       , pi.color.r);
        }

        if (buffer[0])
        {
            m_info.push_back({ Info::Type::Color, true, buffer });
        }
    }

    auto& rc = m_pixelInfo.rc;
    if (rc.isSet())
    {
        rc.normalize();
        const int x = rc.tl.x;
        const int y = rc.tl.y;
        const int w = rc.width();
        const int h = rc.height();

        ::snprintf(buffer, sizeof(buffer), "%d x %d", w, h);
        m_info.push_back({ Info::Type::Size, false, buffer });

        ::snprintf(buffer, sizeof(buffer), "%d, %d -> %d, %d", x, y, x + w - 1, y + h - 1);
        m_info.push_back({ Info::Type::Rect, false, buffer });
    }

    float width = 0;
    for (auto& s : m_info)
    {
        width = std::max<float>(width, m_ft->getStringWidth(s.text.c_str()));
    }

    m_bgSize =
    {
        TextOffsetX + IconsWidth + width + 2.0f * m_border,
        m_rowHeight * m_info.size() + 2.0f * m_border
    };
}

void cPixelPopup::render()
{
    const float mx = ::ceilf(m_pixelInfo.mouse.x);
    const float my = ::ceilf(m_pixelInfo.mouse.y);
    m_pointer->Render(mx - 10.0f, my - 10.0f);

    const bool isInside = isInsideImage(m_pixelInfo.point);
    // if (isInside)
    {
        const auto& viewport = cRenderer::getViewportSize();
        const float x = clamp<float>(0.0f, viewport.x - m_bg->GetWidth(), ::ceilf(mx + InfoOffset));
        const float y = clamp<float>(0.0f, viewport.y - m_bg->GetHeight(), ::ceilf(my + InfoOffset));

        m_bg->SetSpriteSize(m_bgSize.x, m_bgSize.y);
        m_bg->Render(x, y);

        const float startx = x + m_border;
        const float starty = y + m_border;
        for (size_t i = 0, size = m_info.size(); i < size; i++)
        {
            const auto& s = m_info[i];

            const float x = startx + m_iconOffset.x;
            const float y = starty + m_iconOffset.y + i * m_rowHeight;

            m_icons->setColor(s.type == Info::Type::Color && isInside ? m_pixelInfo.color : GrayColor);
            m_icons->setFrame(i);
            m_icons->Render(x, y);

            m_ft->setColor(isInside == s.insideOnly ? WhiteColor : GrayColor);
            m_ft->draw(TextOffsetX + x, TextOffsetY + y, s.text.c_str());
        }
    }
}

bool cPixelPopup::isInsideImage(const Vectorf& pos) const
{
    return !(pos.x < 0 || pos.y < 0 || pos.x >= m_pixelInfo.imgWidth || pos.y >= m_pixelInfo.imgHeight);
}

void cPixelPopup::setCursor(int cursor)
{
    m_pointer->setFrame(cursor);
}
