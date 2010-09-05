/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageborder.h"

#include <GL/gl.h>

CImageBorder::CImageBorder() {
	SetColor(255, 25, 25, 255);
}

CImageBorder::~CImageBorder() {
}

void CImageBorder::SetColor(int r, int g, int b, int a) {
	m_r	= r;
	m_g	= g;
	m_b	= b;
	m_a	= a;
}

void CImageBorder::Render(int x, int y, int w, int h) {
	glColor4ub(m_r, m_g, m_b, m_a);
	glLineWidth(1);
	glBindTexture(GL_TEXTURE_2D, 0);

	renderLine(x - 1, y - 1, x + w + 1, y - 1);	// up
	renderLine(x - 1, y + h, x + w + 1, y + h);	// down
	renderLine(x - 1, y, x - 1, y + h);	// left
	renderLine(x + w, y, x + w, y + h);	// right
}

void CImageBorder::renderLine(float x1, float y1, float x2, float y2) {
	glBegin(GL_LINES);
		glVertex2f(x1 + 0.5f, y1 + 0.5f); glVertex2f(x2 + 0.5f, y2 + 0.5f);
	glEnd();
}
