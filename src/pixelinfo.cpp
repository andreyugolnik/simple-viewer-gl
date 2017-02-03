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

    const int BORDER = 4;
    const int ALPHA = 200;
    const int DesiredFontSize = 13;
    const int FRAME_DELTA = 10;
    const int LINES_COUNT[2] = { 2, 4 };

}

void cPixelInfo::Init()
{
    m_pixelInfo.reset();

    m_bg.reset(new cQuad(0, 0));
    m_bg->SetColor(0, 0, 0, ALPHA);

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
    m_ft->SetColor(255, 255, 255, ALPHA);
}

void cPixelInfo::setPixelInfo(const sPixelInfo& pi)
{
    m_pixelInfo = pi;

    static char info[200];
    if (pi.rc.IsSet())
    {
        const int x = pi.rc.x1;
        const int y = pi.rc.y1;
        const int w = pi.rc.GetWidth();
        const int h = pi.rc.GetHeight();

        ::snprintf(info, sizeof(info),
                 "pos: %d x %d\n" \
                 "argb: 0x%.2x%.2x%.2x%.2x\n" \
                 "size: %d x %d\n" \
                 "rect: %d, %d -> %d, %d"
                 , (int)pi.point.x, (int)pi.point.y
                 , pi.a, pi.r, pi.g, pi.b
                 , w + 1, h + 1
                 , x, y, x + w, y + h);
    }
    else
    {
        ::snprintf(info, sizeof(info),
                 "pos: %d x %d\n" \
                 "argb: 0x%.2x%.2x%.2x%.2x"
                 , (int)pi.point.x, (int)pi.point.y
                 , pi.a, pi.r, pi.g, pi.b);
    }

    m_ft->Update(info);
}

void cPixelInfo::Render()
{
    m_pointer->Render(m_pixelInfo.mouse.x - 10, m_pixelInfo.mouse.y - 10);

    if (isInsideImage(m_pixelInfo.point))
    {
        const int frameWidth = m_ft->GetStringWidth() + 2 * BORDER * m_ratio;
        const int frameHeight = (DesiredFontSize * LINES_COUNT[m_pixelInfo.rc.IsSet()] + 2 * BORDER) * m_ratio;

        const auto& viewport = cRenderer::getViewportSize();
        const int x = std::min<int>(m_pixelInfo.mouse.x + FRAME_DELTA * m_ratio, viewport.x - frameWidth);
        const int y = std::min<int>(m_pixelInfo.mouse.y + FRAME_DELTA * m_ratio, viewport.y - frameHeight);

        m_bg->SetSpriteSize(frameWidth, frameHeight);
        m_bg->Render(x, y);

        m_ft->Render(x + BORDER * m_ratio, y + DesiredFontSize * m_ratio);
    }
}

bool cPixelInfo::isInsideImage(const cVector<float>& pos) const
{
    return !(pos.x < 0 || pos.y < 0 || pos.x >= m_pixelInfo.img_w || pos.y >= m_pixelInfo.img_h);
}

void cPixelInfo::SetCursor(int cursor)
{
    m_pointer->SetFrame(cursor);
}
