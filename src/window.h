/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef WINDOW_H_75C5AF4863DEDB
#define WINDOW_H_75C5AF4863DEDB

#include "math/vector.h"

#include <vector>
#include <memory>

typedef enum
{
    PROP_INFOBAR,
    PROP_PIXELINFO,
    PROP_CHECKERS,
    PROP_FITIMAGE,
    PROP_FULLSCREEN,
    PROP_BORDER,
    PROP_RECURSIVE,
    PROP_CENTER_WINDOW
} Property;

class CQuadImage;
class CFilesList;
class CImageLoader;
class CInfoBar;
class CPixelInfo;
class CCheckerboard;
class CNotAvailable;
class CProgress;
class CImageBorder;
class CSelection;

class CWindow
{
public:
    CWindow();
    virtual ~CWindow();

    bool Init(int argc, char* argv[], const char* path);
    void SetProp(Property prop);
    void SetProp(unsigned char r, unsigned char g, unsigned char b);

private:
    bool m_initialImageLoading;
    float m_scale;
    bool m_windowed;
    bool m_center_window;
    bool m_testFullscreen;
    bool m_fitImage;
    bool m_showBorder;
    bool m_recursiveDir;
    bool m_cursorVisible;
    bool m_mouseLB, m_mouseMB, m_mouseRB;
    cVector m_viewport;
    cVector m_lastMouse;
    cVector m_camera;
    cVector m_prev_pos;
    cVector m_prev_size;
    int m_angle;

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
    void storeWindowPositionSize(bool _position, bool _size);
    void centerWindow();
    void calculateScale();
    void updateScale(bool up);
    void updateFiltering();
    void updateInfobar();
    void createTextures();
    void deleteTextures();
    void calculateTextureSize(int* texW, int* texH, int imgW, int imgH);
    void updatePixelInfo(const cVector& _pos);
    //void updateViewportSize();

    void keyUp();
    void keyDown();
    void keyLeft();
    void keyRight();
    void updatePosition(const cVector& _delta);

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

#endif /* end of include guard: WINDOW_H_75C5AF4863DEDB */

