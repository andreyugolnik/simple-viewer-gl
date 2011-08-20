/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "ftsymbol.h"

CFTSymbol::CFTSymbol(sQuad& _quad, int _tw, int _th, float _tx, float _ty, int _w, int _h)
    : m_w(_w)
    , m_h(_h)
{
    m_quad = _quad;

    float x1 = _tx / _tw;
    float y1 = _ty / _th;
    float x2 = (_tx + _w) / _tw;
    float y2 = (_ty + _h) / _th;

    m_quad.v[0].tx = x1;
    m_quad.v[0].ty = y1;
    m_quad.v[1].tx = x2;
    m_quad.v[1].ty = y1;
    m_quad.v[2].tx = x2;
    m_quad.v[2].ty = y2;
    m_quad.v[3].tx = x1;
    m_quad.v[3].ty = y2;
}

CFTSymbol::~CFTSymbol()
{
}

void CFTSymbol::Render(int _x, int _y)
{
    m_quad.v[0].x = _x;
    m_quad.v[0].y = _y;
    m_quad.v[1].x = _x + m_w;
    m_quad.v[1].y = _y;
    m_quad.v[2].x = _x + m_w;
    m_quad.v[2].y = _y + m_h;
    m_quad.v[3].x = _x;
    m_quad.v[3].y = _y + m_h;

    cRenderer::render(&m_quad);
}

