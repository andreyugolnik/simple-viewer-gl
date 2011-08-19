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

void CImageBorder::SetColor(int r, int g, int b, int a)
{
    m_line.v[0].a = m_line.v[1].a = a;
    m_line.v[0].r = m_line.v[1].r = r;
    m_line.v[0].g = m_line.v[1].g = g;
    m_line.v[0].b = m_line.v[1].b = b;
}

void CImageBorder::Render(int x, int y, int w, int h)
{
    glLineWidth(GetBorderWidth());

    renderLine(x - GetBorderWidth(), y - GetBorderWidth(), x + w + GetBorderWidth(), y - GetBorderWidth());	// up
    renderLine(x - GetBorderWidth(), y + h, x + w + GetBorderWidth(), y + h);	// down
    renderLine(x - GetBorderWidth(), y, x - GetBorderWidth(), y + h);	// left
    renderLine(x + w, y, x + w, y + h);	// right
}

void CImageBorder::renderLine(float x1, float y1, float x2, float y2)
{
    m_line.v[0].x = x1 + 0.5f;
    m_line.v[0].y = y1 + 0.5f;
    m_line.v[1].x = x2 + 0.5f;
    m_line.v[1].y = y2 + 0.5f;
    cRenderer::render(&m_line);
}

int CImageBorder::GetBorderWidth() const
{
    return 1;
}

