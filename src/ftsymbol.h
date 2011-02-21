/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FTSYMBOL_H
#define FTSYMBOL_H

#include <GL/glut.h>

class CFTSymbol {
public:
	CFTSymbol(int tw, int th, float tx, float ty, int w, int h);
	virtual ~CFTSymbol();

	void Render(int x, int y);

private:
	int m_w, m_h;
	struct VERTEX {
		GLint x, y;	// vertex screen coordinates
		GLfloat tx, ty;	// vertex texture coordinates
	} m_v[4];
};

#endif // FTSYMBOL_H
