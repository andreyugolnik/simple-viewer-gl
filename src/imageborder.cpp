/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "imageborder.h"

#include <math.h>

CImageBorder::CImageBorder()
{
    SetColor(25, 255, 25, 255);
}

CImageBorder::~CImageBorder()
{
}

void CImageBorder::SetColor(int r, int g, int b, int a)
{
    m_line.v[0].a = m_line.v[1].a = m_line.v[2].a = m_line.v[3].a = a;
    m_line.v[0].r = m_line.v[1].r = m_line.v[2].r = m_line.v[3].r = r;
    m_line.v[0].g = m_line.v[1].g = m_line.v[2].g = m_line.v[3].g = g;
    m_line.v[0].b = m_line.v[1].b = m_line.v[2].b = m_line.v[3].b = b;
}

void CImageBorder::Render(float x, float y, float w, float h, float scale)
{
    const float delta = GetBorderWidth() / scale;

    renderLine(x - delta, y - delta, w + delta * 2, delta); // up
    renderLine(x - delta, y + h,     w + delta * 2, delta); // down
    renderLine(x - delta, y,         delta,         h); // left
    renderLine(x + w,     y,         delta,         h); // right
}

void CImageBorder::renderLine(float x, float y, float w, float h)
{
    m_line.v[0].x = x;
    m_line.v[0].y = y;
    m_line.v[1].x = x + w;
    m_line.v[1].y = y;
    m_line.v[2].x = x + w;
    m_line.v[2].y = y + h;
    m_line.v[3].x = x;
    m_line.v[3].y = y + h;
    cRenderer::bindTexture(0);
    cRenderer::render(&m_line);
}

float CImageBorder::GetBorderWidth() const
{
    return 2.0f;
}

