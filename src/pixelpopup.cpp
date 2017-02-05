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
#include "types/math.h"

#include <cstring>

namespace
{

    const int AlphaColor = 200;
    const int DesiredFontSize = 13;
    const float Border = 4.0f;
    const float FrameOffset = 10.0f;

}

void cPixelPopup::init()
{
    m_pixelInfo.reset();

    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor({ 0, 0, 0, AlphaColor });

    const int format = imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA;
    m_pointer.reset(new cQuadSeries(imgPointerCross.width, imgPointerCross.height, imgPointerCross.pixel_data, format));
    m_pointer->setup(21, 21, 10);
    setCursor(0);

    createFont();
}

void cPixelPopup::setRatio(float ratio)
{
    if (m_ratio != ratio)
    {
        m_ratio = ratio;
        createFont();
    }
}

void cPixelPopup::createFont()
{
    m_ft.reset(new cFTString(DesiredFontSize * m_ratio));
    m_ft->setColor({ 255, 255, 255, AlphaColor });
}

void cPixelPopup::setPixelInfo(const sPixelInfo& pi)
{
    m_pixelInfo = pi;

    char buffer[100];
    std::vector<std::string> info;

    ::snprintf(buffer, sizeof(buffer), "pos: %d x %d", (int)pi.point.x, (int)pi.point.y);
    info.push_back(buffer);

    char color[20] = { 0 };

    if (pi.bpp == 32)
    {
        ::snprintf(color, sizeof(color), "argb: 0x%.2x%.2x%.2x%.2x"
                 , pi.color.a, pi.color.r, pi.color.g, pi.color.b);
    }
    else if (pi.bpp == 24 || pi.bpp == 16)
    {
        ::snprintf(color, sizeof(color), "rgb: 0x%.2x%.2x%.2x"
                 , pi.color.r, pi.color.g, pi.color.b);
    }
    else if (pi.bpp == 8)
    {
        ::snprintf(color, sizeof(color), "color: 0x%.2x"
                 , pi.color.r);
    }

    if (color[0])
    {
        info.push_back(color);
    }

    auto& rc = m_pixelInfo.rc;
    if (rc.isSet())
    {
        rc.normalize();
        const int x = rc.tl.x;
        const int y = rc.tl.y;
        const int w = rc.width();
        const int h = rc.height();

        ::snprintf(buffer, sizeof(buffer), "size: %d x %d", w, h);
        info.push_back(buffer);

        if (w > 0 && h > 0)
        {
            ::snprintf(buffer, sizeof(buffer), "rect: %d, %d -> %d, %d", x, y, x + w - 1, y + h - 1);
            info.push_back(buffer);
        }
    }

    float width = 0;
    m_text.clear();

    for (auto& s : info)
    {
        width = std::max<float>(width, m_ft->getStringWidth(s.c_str()));
        m_text += s;
        m_text += "\n";
    }

    m_width = width + 2 * Border * m_ratio;
    m_height = (DesiredFontSize * info.size() + 2 * Border) * m_ratio;
}

void cPixelPopup::render()
{
    const float mx = ::ceilf(m_pixelInfo.mouse.x);
    const float my = ::ceilf(m_pixelInfo.mouse.y);
    m_pointer->Render(mx - 10.0f, my - 10.0f);

    if (isInsideImage(m_pixelInfo.point))
    {
        const auto& viewport = cRenderer::getViewportSize();
        const float x = clamp<float>(0.0f, viewport.x - m_bg->GetWidth(), ::ceilf(mx + FrameOffset * m_ratio));
        const float y = clamp<float>(0.0f, viewport.y - m_bg->GetHeight(), ::ceilf(my + FrameOffset * m_ratio));

        m_bg->SetSpriteSize(m_width, m_height);
        m_bg->Render(x, y);

        m_ft->draw(x + Border * m_ratio, y + DesiredFontSize * m_ratio, m_text.c_str());
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
