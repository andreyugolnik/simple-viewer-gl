/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef MAIN_H
#define MAIN_H

const char TITLE[]	= "Simple Viewer GL";

#define	DEF_WINDOW_W	300
#define	DEF_WINDOW_H	200

typedef struct QUAD {
	unsigned int tex;
	struct VERTEX {
		float x, y;
		float tx, ty;
	} v[4];
} Quad;

#endif	// MAIN_H
