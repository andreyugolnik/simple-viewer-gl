/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef MAIN_H
#define MAIN_H

#include <GL/freeglut.h>

const char TITLE[]	= "Simple Viewer GL";

#define	DEF_WINDOW_W	300
#define	DEF_WINDOW_H	200

typedef struct QUAD {
	GLuint tex;
	struct VERTEX {
		GLfloat x, y;
		GLfloat tx, ty;
	} v[4];
} Quad;

#endif	// MAIN_H
