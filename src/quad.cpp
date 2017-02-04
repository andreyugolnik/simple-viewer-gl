/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "quad.h"

#include <cmath>
#include <cstdio>

cQuad::cQuad(unsigned tw, unsigned th, const unsigned char* data, GLenum bitmapFormat)
    : m_tw(tw)
    , m_th(th)
    , m_format(bitmapFormat)
    , m_w(tw)
    , m_h(th)
    , m_filter(true)
{
    m_quad.tex = cRenderer::createTexture();
    setData(data);

    // by deafult set whole texture size
    SetSpriteSize(tw, th);
}

cQuad::~cQuad()
{
    cRenderer::deleteTexture(m_quad.tex);
}

void cQuad::setData(const unsigned char* data)
{
    m_filter = true;
    cRenderer::setData(m_quad.tex, data, m_tw, m_th, m_format);
}

void cQuad::setColor(const cColor& color)
{
    cRenderer::setColor(&m_quad, color);
}

void cQuad::SetTextureRect(float x, float y, float w, float h)
{
    m_w = w;
    m_h = h;

    const float invTw = m_tw ? (1.0f / m_tw) : 0.0f;
    const float invTh = m_th ? (1.0f / m_th) : 0.0f;

    const float tx1 = x * invTw;
    const float ty1 = y * invTh;
    const float tx2 = (x + w) * invTw;
    const float ty2 = (y + h) * invTh;

    m_quad.v[0].tx = tx1;
    m_quad.v[0].ty = ty1;

    m_quad.v[1].tx = tx2;
    m_quad.v[1].ty = ty1;

    m_quad.v[2].tx = tx2;
    m_quad.v[2].ty = ty2;

    m_quad.v[3].tx = tx1;
    m_quad.v[3].ty = ty2;
}

void cQuad::SetSpriteSize(float w, float h)
{
    m_w = w;
    m_h = h;

    const float invTw = m_tw ? (1.0f / m_tw) : 0.0f;
    const float invTh = m_th ? (1.0f / m_th) : 0.0f;

    m_quad.v[0].tx = 0.0f;
    m_quad.v[0].ty = 0.0f;

    m_quad.v[1].tx = w * invTw;
    m_quad.v[1].ty = 0.0f;

    m_quad.v[2].tx = w * invTw;
    m_quad.v[2].ty = h * invTh;

    m_quad.v[3].tx = 0.0f;
    m_quad.v[3].ty = h * invTh;
}

void cQuad::Render(float x, float y)
{
    RenderEx(x, y, m_w, m_h);
}

void cQuad::RenderEx(float x, float y, float w, float h, int angle)
{
    if (angle == 0)
    {
        m_quad.v[0].x = x;
        m_quad.v[0].y = y;
        m_quad.v[1].x = x + w;
        m_quad.v[1].y = y;
        m_quad.v[2].x = x + w;
        m_quad.v[2].y = y + h;
        m_quad.v[3].x = x;
        m_quad.v[3].y = y + h;
    }
    else
    {
        const float a = M_PI * angle / 180.0f;
        const float c = cosf(a);
        const float s = sinf(a);

        m_quad.v[0].x = x;
        m_quad.v[0].y = y;
        m_quad.v[1].x = x + w * c;
        m_quad.v[1].y = y + w * s;
        m_quad.v[2].x = x + w * c - h * s;
        m_quad.v[2].y = y + w * s + h * c;
        m_quad.v[3].x = x - h * s;
        m_quad.v[3].y = y + h * c;
    }

    cRenderer::render(m_quad);
}

void cQuad::useFilter(bool filter)
{
    if (m_filter != filter)
    {
        m_filter = filter;

        cRenderer::bindTexture(m_quad.tex);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter ? GL_LINEAR : GL_NEAREST);
    }
}
