/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef WINDOW_H
#define WINDOW_H

#include "quadimage.h"
#include "fileslist.h"
#include "imageloader.h"
#include "infobar.h"
#include "checkerboard.h"
#include "progress.h"
#include "notavailable.h"
#include "imageborder.h"

#include <vector>

typedef enum { PROP_INFOBAR, PROP_CHECKERS, PROP_FITIMAGE, PROP_FULLSCREEN, PROP_BORDER, PROP_RECURSIVE } Property;

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
	bool m_showBorder;
	bool m_recursiveDir;
	bool m_cusorVisible;
	int m_lastMouseX, m_lastMouseY;
	bool m_mouseLB, m_keyPressed;
	int m_imageDx, m_imageDy;

	GLint m_textureSize;
	int m_quadsCount;
	typedef std::vector<CQuadImage*> Quads;
	typedef Quads::const_iterator QuadsIc;
	Quads m_quads;

	std::auto_ptr<CFilesList> m_filesList;
	std::auto_ptr<CImageLoader> m_il;
	std::auto_ptr<CInfoBar> m_ib;
	std::auto_ptr<CCheckerboard> m_cb;
	std::auto_ptr<CNotAvailable> m_na;
	std::auto_ptr<CProgress> m_progress;
	std::auto_ptr<CImageBorder> m_border;

private:
	bool loadImage(int step);
	void showCursor(bool show);
//	void centerWindow();
	void calculateScale();
	void updateScale(bool up);
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
