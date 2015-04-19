/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef WINDOW_H_75C5AF4863DEDB
#define WINDOW_H_75C5AF4863DEDB

#include "math/vector.h"
#include "callbacks.h"

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
    PROP_CENTER_WINDOW,
    PROP_ALL_VALID
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

class CWindow : public iCallbacks
{
public:
    CWindow();
    virtual ~CWindow();

    bool setInitialImagePath(const char* path);
    void run();

    void SetProp(Property prop);
    void SetProp(unsigned char r, unsigned char g, unsigned char b);

public:
    virtual void doProgress(int percent);

    void fnRender();
    void fnResize(int width, int height);
    void fnMouse(int x, int y);
    void fnMouseButtons(int button, int state, int x, int y);
    void fnMouseWheel(int wheel, int direction, int x, int y);
    void fnKeyboard(unsigned char key, int x, int y);
    void fnKeyboardSpecial(int key, int x, int y);
    void storeWindowPositionSize(bool position, bool size);
    void showCursor(bool show);

private:
    bool loadSubImage(int subStep);
    bool loadImage(int step, int subImage = 0);
    void centerWindow();
    void calculateScale();
    void updateScale(bool up);
    void updateFiltering();
    void updateInfobar();
    void createTextures();
    void deleteTextures();
    void calculateTextureSize(int* texW, int* texH, int imgW, int imgH);
    void updatePixelInfo(const cVector<float>& pos);

    void keyUp();
    void keyDown();
    void keyLeft();
    void keyRight();
    void shiftCamera(const cVector<float>& delta);
    const cVector<float> screenToImage(const cVector<float>& pos);

private:
    bool m_initialImageLoading;
    float m_scale;
    bool m_windowed;
    bool m_center_window;
    bool m_all_valid;
    bool m_testFullscreen;
    bool m_fitImage;
    bool m_showBorder;
    bool m_recursiveDir;
    bool m_cursorVisible;
    bool m_mouseLB, m_mouseMB, m_mouseRB;
    cVector<float> m_lastMouse;
    cVector<float> m_camera;
    cVector<float> m_viewport;
    cVector<float> m_prev_pos;
    cVector<float> m_prev_size;
    int m_angle;

    std::vector<CQuadImage*> m_quads;

    std::unique_ptr<CFilesList> m_filesList;
    std::unique_ptr<CImageLoader> m_imageList;
    std::unique_ptr<CInfoBar> m_infoBar;
    std::unique_ptr<CPixelInfo> m_pixelInfo;
    std::unique_ptr<CCheckerboard> m_checkerBoard;
    std::unique_ptr<CNotAvailable> m_na;
    std::unique_ptr<CProgress> m_progress;
    std::unique_ptr<CImageBorder> m_border;
    std::unique_ptr<CSelection> m_selection;
};

#endif /* end of include guard: WINDOW_H_75C5AF4863DEDB */

