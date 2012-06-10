/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quad.h"

#include <iostream>
#include <math.h>
#include <stdio.h>

CQuad::CQuad(int tw, int th, const unsigned char* data, GLenum bitmapFormat)
    : m_tw(tw)
    , m_th(th)
    , m_w(tw)
    , m_h(th)
    , m_filter(true)
{
    m_quad.tex = cRenderer::createTexture(data, m_w, m_h, bitmapFormat);

    // by deafult set whole texture size
    SetSpriteSize(tw, th);
}

CQuad::~CQuad()
{
    cRenderer::deleteTexture(m_quad.tex);
}

void CQuad::SetColor(int r, int g, int b, int a)
{
    cRenderer::setColor(&m_quad, r, g, b, a);
}

void CQuad::SetSpriteSize(float w, float h)
{
    m_w = w;
    m_h = h;

    m_quad.v[0].tx = 0;
    m_quad.v[0].ty = 0;

    m_quad.v[1].tx = w / m_tw;
    m_quad.v[1].ty = 0;

    m_quad.v[2].tx = w / m_tw;
    m_quad.v[2].ty = h / m_th;

    m_quad.v[3].tx = 0;
    m_quad.v[3].ty = h / m_th;
}

void CQuad::Render(float _x, float _y)
{
    RenderEx(_x, _y, m_w, m_h);
}

void CQuad::RenderEx(float _x, float _y, float w, float h, int angle)
{
    if(angle != 0)
    {
        float a = M_PI * angle / 180;
        float c = cosf(a);
        float s = sinf(a);

        m_quad.v[0].x = _x;
        m_quad.v[0].y = _y;
        m_quad.v[1].x = _x + w*c;
        m_quad.v[1].y = _y + w*s;
        m_quad.v[2].x = _x + w*c - h*s;
        m_quad.v[2].y = _y + w*s + h*c;
        m_quad.v[3].x = _x - h*s;
        m_quad.v[3].y = _y + h*c;
    }
    else
    {
        m_quad.v[0].x = _x;
        m_quad.v[0].y = _y;
        m_quad.v[1].x = _x + w;
        m_quad.v[1].y = _y;
        m_quad.v[2].x = _x + w;
        m_quad.v[2].y = _y + h;
        m_quad.v[3].x = _x;
        m_quad.v[3].y = _y + h;
    }

    cRenderer::render(&m_quad);
}

void CQuad::useFilter(bool _filter)
{
    if(m_filter != _filter)
    {
        m_filter = _filter;

        cRenderer::bindTexture(m_quad.tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter ? GL_LINEAR : GL_NEAREST);
    }
}

