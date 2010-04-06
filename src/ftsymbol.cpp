/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "ftsymbol.h"

CFTSymbol::CFTSymbol(int tw, int th, float tx, float ty, int w, int h) : m_w(w), m_h(h) {
	float x1 = tx / tw;
	float y1 = ty / th;
	float x2 = (tx + w) / tw;
	float y2 = (ty + h) / th;

	m_v[0].tx = x1; m_v[0].ty = y1;
	m_v[1].tx = x2; m_v[1].ty = y1;
	m_v[2].tx = x2; m_v[2].ty = y2;
	m_v[3].tx = x1; m_v[3].ty = y2;
}

CFTSymbol::~CFTSymbol() {
}

void CFTSymbol::Render(int x, int y) {
	m_v[0].x = x;		m_v[0].y = y;
	m_v[1].x = x + m_w;	m_v[1].y = y;
	m_v[2].x = x + m_w;	m_v[2].y = y + m_h;
	m_v[3].x = x;		m_v[3].y = y + m_h;

	glBegin(GL_QUADS);
		glTexCoord2fv(&m_v[0].tx);	glVertex2iv(&m_v[0].x);
		glTexCoord2fv(&m_v[1].tx);	glVertex2iv(&m_v[1].x);
		glTexCoord2fv(&m_v[2].tx);	glVertex2iv(&m_v[2].x);
		glTexCoord2fv(&m_v[3].tx);	glVertex2iv(&m_v[3].x);
	glEnd();
}
