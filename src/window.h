/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef WINDOW_H
#define WINDOW_H

#include "fileslist.h"
#include "imageloader.h"
#include "infobar.h"
#include "checkerboard.h"
#include "main.h"

#include <GL/freeglut.h>
#include <vector>

typedef enum { PROP_INFOBAR, PROP_CHECKERS, PROP_FITIMAGE, PROP_FULLSCREEN } Property;

class CWindow {
public:
	CWindow();
	virtual ~CWindow();

	bool Init(int argc, char *argv[], const char* path);
	void SetProp(Property prop);
	void SetProp(unsigned char r, unsigned char g, unsigned char b);

private:
	int m_winW, m_winH;
	float m_scale;
	bool m_windowed;
	bool m_fitImage;
	bool m_cusorVisible;
	int m_lastMouseX, m_lastMouseY;
	bool m_mouseLB, m_keyPressed;
	int m_mouseDx, m_mouseDy;
	bool m_renderNa;

	GLint m_textureSize;
	int m_quadsCount;
	typedef struct QUAD_IMG : Quad {
		int w, h;
		int col, row;
	} QuadImg;
	typedef std::vector<QuadImg> Quads;
	typedef Quads::const_iterator QuadsIc;
	Quads m_quads;
	int m_loadingTime;

	std::auto_ptr<CFilesList> m_filesList;
	std::auto_ptr<CImageLoader> m_il;
	std::auto_ptr<CInfoBar> m_ib;
	std::auto_ptr<CCheckerboard> m_cb;

private:
	bool loadImage(int step);
	void showCursor(bool show);
//	void centerWindow();
	void calculateScale();
	void updateInfobar();
	void createTextures(int width, int height, bool alpha, unsigned char* bitmap);
	void copyBuffer(unsigned char* bitmap, int col, int row, int width, bool alpha, unsigned char* buffer, int w, int h);

	void fnProgressLoading(Imlib_Image im, char percent, int update_x, int update_y, int update_w, int update_h);
	static int callbackProgressLoading(Imlib_Image im, char percent, int update_x, int update_y, int update_w, int update_h);

	void fnRender();
	void fnResize(int width, int height);
	void fnMouse(int x, int y);
	void fnMouseButtons(int button, int state, int x, int y);
	void fnMouseWheel(int wheel, int direction, int x, int y);
	void fnKeyboard(unsigned char key, int x, int y);
	void fnKeyboardSpecial(int key, int x, int y);

	static void callbackRender();
	static void callbackResize(int width, int height);
	static void callbackTimerCursor(int value);
	static void callbackMouse(int x, int y);
	static void callbackMouseButtons(int button, int state, int x, int y);
	static void callbackMouseWheel(int wheel, int direction, int x, int y);
	static void callbackKeyboard(unsigned char key, int x, int y);
	static void callbackKeyboardSpecial(int key, int x, int y);
};

#endif // WINDOW_H
