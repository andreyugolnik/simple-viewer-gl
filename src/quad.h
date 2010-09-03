/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUAD_H
#define QUAD_H

#include "rect.h"
#include <GL/freeglut.h>

class CQuad {
public:
	CQuad(int tw, int th, const unsigned char* data = 0, GLenum bitmapFormat = GL_RGB);
	virtual ~CQuad();

	virtual void SetSpriteSize(float w, float h);
	virtual void SetWindowSize(float w, float h);
	virtual void Render(float x, float y);
	virtual void RenderEx(float x, float y, float w, float h, int rot = 0);
	virtual float GetWidth() const { return m_w; }
	virtual float GetHeight() const { return m_h; }
	virtual float GetTexWidth() const { return m_tw; }
	virtual float GetTexHeight() const { return m_th; }

protected:
	GLfloat m_tw, m_th;	// texture width / height
	GLuint m_tex;
	GLfloat m_w, m_h;	// sprite width / height
	CRect<float> m_rcWindow;	// current window size
	struct VERTEX {
		GLfloat x, y;	// vertex screen coordinates
		GLfloat tx, ty;	// vertex texture coordinates
	} m_v[4];


private:
	CQuad();
};

#endif // QUAD_H
