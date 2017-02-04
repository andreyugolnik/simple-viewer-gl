/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "pixelinfo.h"
#include "img-pointer-cross.c"

#include <cstring>

namespace
{

    const int Border = 4;
    const int AlphaColor = 200;
    const int DesiredFontSize = 13;
    const int FRAME_DELTA = 10;

}

void cPixelInfo::Init()
{
    m_pixelInfo.reset();

    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor({ 0, 0, 0, AlphaColor });

    const int format = imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA;
    m_pointer.reset(new cQuadSeries(imgPointerCross.width, imgPointerCross.height, imgPointerCross.pixel_data, format));
    m_pointer->Setup(21, 21, 10);
    SetCursor(0);

    createFont();
}

void cPixelInfo::setRatio(float ratio)
{
    if (m_ratio != ratio)
    {
        m_ratio = ratio;
        createFont();
    }
}

void cPixelInfo::createFont()
{
    m_ft.reset(new cFTString(DesiredFontSize * m_ratio));
    m_ft->setColor({ 255, 255, 255, AlphaColor });
}

void cPixelInfo::setPixelInfo(const sPixelInfo& pi)
{
    m_pixelInfo = pi;

    char buffer[100];
    std::vector<std::string> info;

    ::snprintf(buffer, sizeof(buffer), "pos: %d x %d", (int)pi.point.x, (int)pi.point.y);
    info.push_back(buffer);

    char color[20] = { 0 };

    if (pi.bpp == 32)
    {
        ::snprintf(color, sizeof(color), "argb: 0x%.2x%.2x%.2x%.2x\n"
                 , pi.a, pi.r, pi.g, pi.b);
    }
    else if (pi.bpp == 24 || pi.bpp == 16)
    {
        ::snprintf(color, sizeof(color), "rgb: 0x%.2x%.2x%.2x"
                 , pi.r, pi.g, pi.b);
    }
    else if (pi.bpp == 8)
    {
        ::snprintf(color, sizeof(color), "color: 0x%.2x"
                 , pi.r);
    }

    if (color[0])
    {
        info.push_back(color);
    }

    if (pi.rc.IsSet())
    {
        const int x = pi.rc.x1;
        const int y = pi.rc.y1;
        const int w = pi.rc.GetWidth();
        const int h = pi.rc.GetHeight();

        ::snprintf(buffer, sizeof(buffer), "size: %d x %d", w + 1, h + 1);
        info.push_back(buffer);

        ::snprintf(buffer, sizeof(buffer), "rect: %d, %d -> %d, %d", x, y, x + w, y + h);
        info.push_back(buffer);
    }

    float width = 0;
    m_text.clear();

    for (auto& s : info)
    {
        width = std::max<float>(width, m_ft->getStringWidth(s.c_str()));
        m_text += s;
        m_text += "\n";
    }

    width += 2 * Border * m_ratio;
    const float height = (DesiredFontSize * info.size() + 2 * Border) * m_ratio;
    m_bg->SetSpriteSize(width, height);
}

void cPixelInfo::Render()
{
    m_pointer->Render(m_pixelInfo.mouse.x - 10, m_pixelInfo.mouse.y - 10);

    if (isInsideImage(m_pixelInfo.point))
    {
        const auto& viewport = cRenderer::getViewportSize();
        const int x = std::min<int>(m_pixelInfo.mouse.x + FRAME_DELTA * m_ratio, viewport.x - m_bg->GetWidth());
        const int y = std::min<int>(m_pixelInfo.mouse.y + FRAME_DELTA * m_ratio, viewport.y - m_bg->GetHeight());

        m_bg->Render(x, y);

        m_ft->draw(x + Border * m_ratio, y + DesiredFontSize * m_ratio, m_text.c_str());
    }
}

bool cPixelInfo::isInsideImage(const cVector<float>& pos) const
{
    return !(pos.x < 0 || pos.y < 0 || pos.x >= m_pixelInfo.imgWidth || pos.y >= m_pixelInfo.imgHeight);
}

void cPixelInfo::SetCursor(int cursor)
{
    m_pointer->SetFrame(cursor);
}
