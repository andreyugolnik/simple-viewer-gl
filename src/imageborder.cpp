/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageborder.h"

CImageBorder::CImageBorder()
{
    SetColor(255, 25, 25, 255);
}

CImageBorder::~CImageBorder()
{
}

void CImageBorder::SetColor(int _r, int _g, int _b, int _a)
{
    m_line.v[0].a = m_line.v[1].a = _a;
    m_line.v[0].r = m_line.v[1].r = _r;
    m_line.v[0].g = m_line.v[1].g = _g;
    m_line.v[0].b = m_line.v[1].b = _b;
}

void CImageBorder::Render(int _w, int _h)
{
    glLineWidth(GetBorderWidth());

    renderLine(-GetBorderWidth(), -GetBorderWidth(), _w + GetBorderWidth(), -GetBorderWidth());	// up
    renderLine(-GetBorderWidth(), _h, _w + GetBorderWidth(), _h);	// down
    renderLine(-GetBorderWidth(), 0, -GetBorderWidth(), _h);	// left
    renderLine(_w, 0, _w, _h);	// right
}

void CImageBorder::renderLine(float _x1, float _y1, float _x2, float _y2)
{
    m_line.v[0].x = _x1 + 0.5f;
    m_line.v[0].y = _y1 + 0.5f;
    m_line.v[1].x = _x2 + 0.5f;
    m_line.v[1].y = _y2 + 0.5f;
    cRenderer::render(&m_line);
}

int CImageBorder::GetBorderWidth() const
{
    return 1;
}

