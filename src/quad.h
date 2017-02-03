/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "renderer.h"

class cQuad
{
public:
    cQuad(unsigned tw, unsigned th, const unsigned char* data = 0, GLenum bitmapFormat = GL_RGB);
    virtual ~cQuad();

    virtual void setData(const unsigned char* data);
    virtual void SetColor(int r, int g, int b, int a);
    virtual void SetTextureRect(float x, float y, float w, float h);
    virtual void SetSpriteSize(float w, float h);
    virtual void Render(float x, float y);
    virtual void RenderEx(float x, float y, float w, float h, int rot = 0);

    virtual unsigned GetTexWidth() const
    {
        return m_tw;
    }
    virtual unsigned GetTexHeight() const
    {
        return m_th;
    }

    virtual float GetWidth() const
    {
        return m_w;
    }
    virtual float GetHeight() const
    {
        return m_h;
    }

    virtual void useFilter(bool filter);

    GLenum getFormat() const
    {
        return m_format;
    }

protected:
    // texture size
    unsigned m_tw;
    unsigned m_th;
    GLenum m_format;

    // sprite size
    float m_w;
    float m_h;

    bool m_filter;

    sQuad m_quad;
};
