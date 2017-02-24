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

    const float TextOffset = 16.0f;

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

void cPixelPopup::setScale(float scale)
{
    if (m_scale != scale)
    {
        const float RowHeight = 36.0f;
        m_rowHeight = RowHeight * scale;

        const float Border = 10.0f;
        m_border = Border * scale;

        const int desiredFontSize = 30;
        createFont(desiredFontSize * scale);

        m_scale = scale;
    }
}

void cPixelPopup::createFont(int fontSize)
{
    m_ft.reset(new cFTString(fontSize));
}

void cPixelPopup::setPixelInfo(const sPixelInfo& pi)
{
    m_pixelInfo = pi;

    const bool isInside = isInsideImage(m_pixelInfo.point);

    char buffer[100];
    m_info.clear();

    ::snprintf(buffer, sizeof(buffer), "%d x %d", (int)pi.point.x, (int)pi.point.y);
    m_info.push_back({ Info::Icon::Position, isInside ? WhiteColor : GrayColor, buffer, {} });

    if (isInside)
    {
        const auto& c = pi.color;
        ::snprintf(buffer, sizeof(buffer), "rgba %.2X %.2X %.2X %.2X"
                   , c.r, c.g, c.b, c.a);
        m_info.push_back({ Info::Icon::Color, WhiteColor, buffer, {} });
    }
    else
    {
        m_info.push_back({ Info::Icon::Color, GrayColor, "rgba - - - -", {} });
    }

    auto& rc = m_pixelInfo.rc;
    if (rc.isSet())
    {
        rc.normalize();
        const int x = (int)rc.tl.x;
        const int y = (int)rc.tl.y;
        const int w = (int)rc.width();
        const int h = (int)rc.height();

        ::snprintf(buffer, sizeof(buffer), "%d x %d", w, h);
        m_info.push_back({ Info::Icon::Size, WhiteColor, buffer, {} });

        ::snprintf(buffer, sizeof(buffer), "%d, %d -> %d, %d", x, y, x + w - 1, y + h - 1);
        m_info.push_back({ Info::Icon::Rect, WhiteColor, buffer, {} });
    }

    float width = 0;
    for (auto& s : m_info)
    {
        auto bounds = m_ft->getBounds(s.text.c_str());
        width = std::max<float>(width, bounds.x);

        s.offset =
        {
            IconsWidth + TextOffset,
            (m_rowHeight - bounds.y) * 0.5f - 4.0f
        };
    }

    m_bgSize =
    {
        IconsWidth + TextOffset + width + 2.0f * m_border,
        m_rowHeight * m_info.size() + 2.0f * m_border
    };
}

void cPixelPopup::render()
{
    const float mx = ::roundf(m_pixelInfo.mouse.x);
    const float my = ::roundf(m_pixelInfo.mouse.y);
    m_pointer->render({ mx - 10.0f, my - 10.0f });

    const auto& viewport = cRenderer::getViewportSize();
    const auto& size = m_bg->getSize();

    Vectorf pos
    {
        clamp<float>(0.0f, viewport.x - size.x, ::roundf(mx + InfoOffset)),
        clamp<float>(0.0f, viewport.y - size.y, ::roundf(my + InfoOffset))
    };

    m_bg->setSpriteSize(m_bgSize);
    m_bg->render(pos);

    pos += Vectorf{ m_border, m_border };
    const Vectorf iconOffset{ 0.0f, (m_rowHeight - IconsHeight) * 0.5f };
    for (const auto& s : m_info)
    {
        // m_icons->setColor(s.Icon == Info::Icon::Color && isInside ? m_pixelInfo.color : GrayColor);
        m_icons->setFrame((uint32_t)s.icon);
        m_icons->render(pos + iconOffset);

        m_ft->setColor(s.color);
        m_ft->draw(pos + s.offset, s.text.c_str());

        pos.y += m_rowHeight;
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
