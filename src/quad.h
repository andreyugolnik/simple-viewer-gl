/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUAD_H
#define QUAD_H

#include <GL/freeglut.h>

class CQuad {
public:
	CQuad(int tw, int th, const unsigned char* data = 0, int bpp = 0);
	virtual ~CQuad();

	virtual void SetSpriteSize(float w, float h);
	virtual void Render(float x, float y);

private:
	GLuint m_tex;
	GLfloat m_w, m_h;	// sprite width / height
	GLfloat m_tw, m_th;	// initial texture width / height
	struct VERTEX {
		GLfloat x, y;	// vertex screen coordinates
		GLfloat tx, ty;	// vertex texture coordinates
	} m_v[4];

private:
	CQuad();
	void init(int tw, int th, const unsigned char* data, int bpp);
};

#endif // QUAD_H
