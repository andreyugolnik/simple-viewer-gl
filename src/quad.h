/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUAD_H
#define QUAD_H

#include "rect.h"
#include "renderer.h"

class CQuad
{
public:
    CQuad(int tw, int th, const unsigned char* data = 0, GLenum bitmapFormat = GL_RGB);
    virtual ~CQuad();

    virtual void SetColor(int r, int g, int b, int a);
    virtual void SetSpriteSize(float w, float h);
    virtual void SetWindowSize(float w, float h);
    virtual void Render(float x, float y);
    virtual void RenderEx(float x, float y, float w, float h, int rot = 0);
    virtual float GetWidth() const { return m_w; }
    virtual float GetHeight() const { return m_h; }
    virtual float GetTexWidth() const { return m_tw; }
    virtual float GetTexHeight() const { return m_th; }
    virtual void useFilter(bool _filter);

protected:
    GLfloat m_tw, m_th;	// texture width / height
    GLfloat m_w, m_h;	// sprite width / height
    CRect<float> m_rcWindow;	// current window size
    sQuad m_quad;
    bool m_filter;

private:
    CQuad();
};

#endif // QUAD_H

