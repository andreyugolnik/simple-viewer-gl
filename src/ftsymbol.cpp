/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ftsymbol.h"

cFTSymbol::cFTSymbol(GLuint texId, float tw, float th, float tx, float ty, float w, float h)
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

void cFTSymbol::render(const Vectorf& pos, const cColor& color, float scale)
{
    m_quad.v[0].x = pos.x;
    m_quad.v[0].y = pos.y;
    m_quad.v[0].color = color;
    m_quad.v[1].x = pos.x + m_w * scale;
    m_quad.v[1].y = pos.y;
    m_quad.v[1].color = color;
    m_quad.v[2].x = pos.x + m_w * scale;
    m_quad.v[2].y = pos.y + m_h * scale;
    m_quad.v[2].color = color;
    m_quad.v[3].x = pos.x;
    m_quad.v[3].y = pos.y + m_h * scale;
    m_quad.v[3].color = color;

    cRenderer::render(m_quad);
}
