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

void CQuad::SetWindowSize(float w, float h)
{
    m_rcWindow.Set(0, 0, w, h);
}

void CQuad::Render(float x, float y)
{
    RenderEx(x, y, m_w, m_h);
}

void CQuad::RenderEx(float x, float y, float w, float h, int angle)
{
    if(angle != 0)
    {
        float a = M_PI * angle / 180;
        float c = cosf(a);
        float s = sinf(a);

        m_quad.v[0].x = x;
        m_quad.v[0].y = y;
        m_quad.v[1].x = x + w*c;
        m_quad.v[1].y = y + w*s;
        m_quad.v[2].x = x + w*c - h*s;
        m_quad.v[2].y = y + w*s + h*c;
        m_quad.v[3].x = x - h*s;
        m_quad.v[3].y = y + h*c;
    }
    else
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

    CRect<float> rc;
    rc.Encapsulate(m_quad.v[0].x, m_quad.v[0].y);
    rc.Encapsulate(m_quad.v[1].x, m_quad.v[1].y);
    rc.Encapsulate(m_quad.v[2].x, m_quad.v[2].y);
    rc.Encapsulate(m_quad.v[3].x, m_quad.v[3].y);

    if(rc.IsSet() == false || m_rcWindow.IsSet() == false || rc.Intersect(&m_rcWindow) == true)
    {
        cRenderer::render(&m_quad);
    }
}

