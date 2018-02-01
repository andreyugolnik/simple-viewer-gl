/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "imageborder.h"

#include <cmath>

cImageBorder::cImageBorder()
{
    setColor({ 25, 255, 25, 255 });
}

cImageBorder::~cImageBorder()
{
}

void cImageBorder::setColor(const cColor& color)
{
    m_line.v[0].color = m_line.v[1].color = m_line.v[2].color = m_line.v[3].color = color;
}

void cImageBorder::render(float x, float y, float w, float h)
{
    const float delta = getThickness() / cRenderer::getZoom();

    renderLine(x - delta, y - delta, w + delta * 2, delta); // up
    renderLine(x - delta,     y + h, w + delta * 2, delta); // down
    renderLine(x - delta,         y,         delta,     h); // left
    renderLine(    x + w,         y,         delta,     h); // right
}

void cImageBorder::renderLine(float x, float y, float w, float h)
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
    cRenderer::render(m_line);
}
