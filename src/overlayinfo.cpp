/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "overlayinfo.h"

#include <cstring>

namespace
{

    const int Border = 4.0f;
    const int ALPHA = 200;
    const int DesiredFontSize = 13;

}

void cOverlayInfo::init()
{
    m_bg.reset(new cQuad(0, 0));
    m_bg->SetColor(0, 0, 0, ALPHA);

    createFont();
}

void cOverlayInfo::setRatio(float ratio)
{
    if (m_ratio != ratio)
    {
        m_ratio = ratio;
        createFont();
    }
}

void cOverlayInfo::createFont()
{
    m_ft.reset(new cFTString(DesiredFontSize * m_ratio));
    m_ft->SetColor(255, 255, 255, ALPHA);
}

void cOverlayInfo::setData(const char* data)
{
    m_rows = 0;
    for (auto p = data; *p != 0; p++)
    {
        if (*p == '\n')
        {
            m_rows++;
        }
    }

    m_ft->Update(data);
}

void cOverlayInfo::render()
{
    if (m_rows)
    {
        const float frameWidth = m_ft->GetStringWidth() + 2.0f * Border * m_ratio;
        const float frameHeight = (DesiredFontSize * m_rows + 2.0f * Border) * m_ratio;

        const float x = 5.0f;
        const float y = 5.0f;

        m_bg->SetSpriteSize(frameWidth, frameHeight);
        m_bg->Render(x, y);

        m_ft->Render(x + Border * m_ratio, y + DesiredFontSize * m_ratio);
    }
}
