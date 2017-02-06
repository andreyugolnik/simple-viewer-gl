/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ftsymbol.h"

cFTSymbol::cFTSymbol(GLuint texId, int tw, int th, float tx, float ty, int w, int h)
    : m_w(w)
    , m_h(h)
{
    const float x1 = tx / tw;
    const float y1 = ty / th;
    const float x2 = (tx + w) / tw;
    const float y2 = (ty + h) / th;

    m_quad.tex = texId;
    m_quad.v[0].tx = x1;
    m_quad.v[0].ty = y1;
    m_quad.v[1].tx = x2;
    m_quad.v[1].ty = y1;
    m_quad.v[2].tx = x2;
    m_quad.v[2].ty = y2;
    m_quad.v[3].tx = x1;
    m_quad.v[3].ty = y2;
}

cFTSymbol::~cFTSymbol()
{
}

void cFTSymbol::render(int x, int y, const cColor& color)
{
    m_quad.v[0].x = x;
    m_quad.v[0].y = y;
    m_quad.v[0].color = color;
    m_quad.v[1].x = x + m_w;
    m_quad.v[1].y = y;
    m_quad.v[1].color = color;
    m_quad.v[2].x = x + m_w;
    m_quad.v[2].y = y + m_h;
    m_quad.v[2].color = color;
    m_quad.v[3].x = x;
    m_quad.v[3].y = y + m_h;
    m_quad.v[3].color = color;

    cRenderer::render(m_quad);
}
