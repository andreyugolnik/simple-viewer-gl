/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "pixelpopup.h"
#include "img-icons.c"
#include "img-pointer-cross.c"
#include "types/math.h"

#include <cstring>

namespace
{
    const ImVec4 GrayColor{ 0.6f, 0.6f, 0.6f, 1.0f };
    const ImVec4 WhiteColor{ 1.0f, 1.0f, 1.0f, 1.0f };
}

void cPixelPopup::init()
{
    m_pixelInfo.reset();

    m_pointer.reset(new cQuadSeries(imgPointerCross.width, imgPointerCross.height, imgPointerCross.pixel_data, imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA));
    m_pointer->setup(21, 21, 10);
    setCursor(0);

    m_icons.reset(new cQuadSeries(imgIcons.width, imgIcons.height, imgIcons.pixel_data, imgIcons.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA));
    m_icons->setup(16, 16, 4);
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
        ::snprintf(buffer, sizeof(buffer), "rgba %.2X %.2X %.2X %.2X", c.r, c.g, c.b, c.a);
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
}

void cPixelPopup::render()
{
    const float mx = ::roundf(m_pixelInfo.mouse.x);
    const float my = ::roundf(m_pixelInfo.mouse.y);
    m_pointer->render({ mx - 10.0f, my - 10.0f });

    const auto& size = m_icons->getSize();
    ImGui::BeginTooltip();
    for (const auto& s : m_info)
    {
        m_icons->setFrame((uint32_t)s.icon);
        auto& quad = m_icons->getQuad();
        ImGui::Image((void*)(uintptr_t)quad.tex, { size.x, size.y }, { quad.v[0].tx, quad.v[0].ty }, { quad.v[2].tx, quad.v[2].ty });
        ImGui::SameLine();
        ImGui::TextColored(s.color, "%s", s.text.c_str());
    }
    ImGui::EndTooltip();
}

bool cPixelPopup::isInsideImage(const Vectorf& pos) const
{
    return !(pos.x < 0 || pos.y < 0 || pos.x >= m_pixelInfo.imgWidth || pos.y >= m_pixelInfo.imgHeight);
}

void cPixelPopup::setCursor(int cursor)
{
    m_pointer->setFrame(cursor);
}
