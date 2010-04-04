/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <GL/freeglut.h>

class CCheckerboard {
public:
	CCheckerboard();
	virtual ~CCheckerboard();

	void Render();
	void RenderLoading();
	void RenderNa();
	void Enable(bool enable = true) { m_enabled = enable; }
	bool IsEnabled() const { return m_enabled; }

private:
	bool m_enabled;
	GLfloat r, g, b;
	typedef struct QUAD {
		GLuint tex;
		struct VERTEX {
			GLfloat x, y;
			GLfloat tx, ty;
		} v[4];
	} Quad;
	Quad m_cb;
	Quad m_loading;
	Quad m_na;
};

#endif // CHECKERBOARD_H
