/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include "main.h"
#include <GL/freeglut.h>

class CCheckerboard {
public:
	CCheckerboard();
	virtual ~CCheckerboard();

	void Init();
	void Render();
	void RenderLoading();
	void RenderNa();
	void Enable(bool enable = true) { m_enabled = enable; }
	bool IsEnabled() const { return m_enabled; }
	void SetColor(unsigned char r, unsigned char g, unsigned char b);

private:
	bool m_enabled;
	GLfloat m_r, m_g, m_b;
	Quad m_cb;
	Quad m_loading;
	Quad m_na;
};

#endif // CHECKERBOARD_H
