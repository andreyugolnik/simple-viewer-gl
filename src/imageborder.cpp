/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageborder.h"

#include <math.h>

CImageBorder::CImageBorder()
{
    SetColor(255, 25, 25, 255);
}

CImageBorder::~CImageBorder()
{
}

void CImageBorder::SetColor(int _r, int _g, int _b, int _a)
{
    m_line.v[0].a = m_line.v[1].a = m_line.v[2].a = m_line.v[3].a = _a;
    m_line.v[0].r = m_line.v[1].r = m_line.v[2].r = m_line.v[3].r = _r;
    m_line.v[0].g = m_line.v[1].g = m_line.v[2].g = m_line.v[3].g = _g;
    m_line.v[0].b = m_line.v[1].b = m_line.v[2].b = m_line.v[3].b = _b;
}

void CImageBorder::Render(float _x, float _y, float _w, float _h, float _scale)
{
    _x *= _scale;
    _y *= _scale;
    _w = _w * _scale;
    _h = _h * _scale;

    const float delta = GetBorderWidth();

    renderLine(_x - delta, _y - delta, _w + delta*2, delta);	// up
    renderLine(_x - delta, _y + _h,    _w + delta*2, delta);	// down
    renderLine(_x - delta, _y, delta, _h);	// left
    renderLine(_x + _w,    _y, delta, _h);	// right
}

void CImageBorder::renderLine(float _x, float _y, float _w, float _h)
{
    m_line.v[0].x = _x;
    m_line.v[0].y = _y;
    m_line.v[1].x = _x + _w;
    m_line.v[1].y = _y;
    m_line.v[2].x = _x + _w;
    m_line.v[2].y = _y + _h;
    m_line.v[3].x = _x;
    m_line.v[3].y = _y + _h;
    cRenderer::bindTexture(0);
    cRenderer::render(&m_line);
}

int CImageBorder::GetBorderWidth() const
{
    return 2;
}

