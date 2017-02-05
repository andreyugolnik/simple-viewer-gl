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
    cQuad(uint32_t tw, uint32_t th, const uint8_t* data = 0, GLenum bitmapFormat = GL_RGB);
    virtual ~cQuad();

    virtual void setData(const uint8_t* data);
    virtual void setColor(const cColor& color);
    virtual void SetTextureRect(float x, float y, float w, float h);
    virtual void SetSpriteSize(float w, float h);
    virtual void Render(float x, float y);
    virtual void RenderEx(float x, float y, float w, float h, int rot = 0);

    virtual uint32_t GetTexWidth() const
    {
        return m_tw;
    }
    virtual uint32_t GetTexHeight() const
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
    uint32_t m_tw;
    uint32_t m_th;
    GLenum m_format;

    // sprite size
    float m_w;
    float m_h;

    bool m_filter;

    sQuad m_quad;
};
