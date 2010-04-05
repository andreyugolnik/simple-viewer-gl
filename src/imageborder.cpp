/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageborder.h"

CImageBorder::CImageBorder() : m_width(1) {
	SetColor(1, 0.1f, 0.1f, 1);
}

CImageBorder::~CImageBorder() {
}

void CImageBorder::SetColor(float r, float g, float b, float a) {
	v[0].r = r; v[0].g = g; v[0].b = b; v[0].a = a;
	v[1].r = r; v[1].g = g; v[1].b = b; v[1].a = a;
}

void CImageBorder::Render(float x, float y, float w, float h) {
	glLineWidth(m_width);
	glBindTexture(GL_TEXTURE_2D, 0);

	renderLine(x, y, x + w, y);	// up
	renderLine(x, y, x, y + h);	// left
	renderLine(x + w, y, x + w, y + h);	// right
	renderLine(x, y + h, x + w, y + h);	// down
}

void CImageBorder::renderLine(float x1, float y1, float x2, float y2) {
	v[0].x	= x1 + 0.5f;	v[0].y	= y1 + 0.5f;
	v[1].x	= x2 + 0.5f;	v[1].y	= y2 + 0.5f;

	glBegin(GL_LINES);
		glColor4fv(&v[0].r), glVertex3fv(&v[0].x);
		glColor4fv(&v[1].r), glVertex3fv(&v[1].x);
	glEnd();
}
