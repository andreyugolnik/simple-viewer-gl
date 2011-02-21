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
#include "pixelinfo.h"
#include "checkerboard.h"
#include "progress.h"
#include "notavailable.h"
#include "imageborder.h"
#include "selection.h"

#include <vector>

typedef enum { PROP_INFOBAR, PROP_PIXELINFO, PROP_CHECKERS, PROP_FITIMAGE, PROP_FULLSCREEN, PROP_BORDER, PROP_RECURSIVE } Property;

class CWindow
{
public:
    CWindow();
    virtual ~CWindow();

    bool Init(int argc, char *argv[], const char* path);
    void SetProp(Property prop);
    void SetProp(unsigned char r, unsigned char g, unsigned char b);

private:
    static CWindow* m_window;
    bool m_initialImageLoading;
    int m_prevWinX, m_prevWinY, m_prevWinW, m_prevWinH;
    int m_curWinW, m_curWinH;
    float m_scale;
    bool m_windowed;
    bool m_testFullscreen;
    bool m_fitImage;
    bool m_showBorder;
    bool m_recursiveDir;
    bool m_cursorVisible;
    int m_lastMouseX, m_lastMouseY;
    bool m_mouseLB, m_mouseMB, m_mouseRB, m_keyPressed;
    int m_imageDx, m_imageDy;

    bool m_pow2;
    GLint m_textureSize;
    typedef std::vector<CQuadImage*> Quads;
    typedef Quads::const_iterator QuadsIc;
    Quads m_quads;

    std::auto_ptr<CFilesList> m_filesList;
    std::auto_ptr<CImageLoader> m_imageList;
    std::auto_ptr<CInfoBar> m_infoBar;
    std::auto_ptr<CPixelInfo> m_pixelInfo;
    std::auto_ptr<CCheckerboard> m_checkerBoard;
    std::auto_ptr<CNotAvailable> m_na;
    std::auto_ptr<CProgress> m_progress;
    std::auto_ptr<CImageBorder> m_border;
    std::auto_ptr<CSelection> m_selection;

private:
    bool loadImage(int step, int subImage = 0);
    void showCursor(bool show);
    void centerWindow();
    void calculateScale();
    void updateScale(bool up);
    void updateInfobar();
    void createTextures();
    void deleteTextures();
    void calculateTextureSize(int* texW, int* texH, int imgW, int imgH);
    void updatePixelInfo(int x, int y);
    void updateViewportSize();

    void fnProgressLoading(int percent);
    static void callbackProgressLoading(int percent);

    void fnRender();
    void fnResize(int width, int height);
    void fnMouse(int x, int y);
    void fnMouseButtons(int button, int state, int x, int y);
    void fnMouseWheel(int wheel, int direction, int x, int y);
    void fnKeyboard(unsigned char key, int x, int y);
    void fnKeyboardSpecial(int key, int x, int y);

    static void callbackRender();
    static void callbackTimerUpdate(int value);
    static void callbackResize(int width, int height);
    static void callbackTimerCursor(int value);
    static void callbackMouse(int x, int y);
    static void callbackMouseButtons(int button, int state, int x, int y);
    static void callbackMouseWheel(int wheel, int direction, int x, int y);
    static void callbackKeyboard(unsigned char key, int x, int y);
    static void callbackKeyboardSpecial(int key, int x, int y);
};

#endif // WINDOW_H

