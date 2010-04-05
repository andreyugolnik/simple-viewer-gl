/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGEBORDER_H
#define IMAGEBORDER_H

#include <GL/freeglut.h>

class CImageBorder {
public:
	CImageBorder();
	virtual ~CImageBorder();

	void SetWidth(float width) { m_width = width; }
	void SetColor(float r, float g, float b, float a);
	void Render(float x, float y, float w, float h);

private:
	float m_width;

	struct LINE_VERTEX {
		GLfloat x, y;
		GLfloat r, g, b, a;
	} v[2];

private:
	void renderLine(float x1, float y1, float x2, float y2);
};

#endif // IMAGEBORDER_H
