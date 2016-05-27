/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "math/vector.h"
#include "callbacks.h"

#include <vector>
#include <memory>

class CQuadImage;
class CFilesList;
class CImageLoader;
class CInfoBar;
class CPixelInfo;
class CCheckerboard;
class CProgress;
class CImageBorder;
class CSelection;

class GLFWwindow;

class cViewer final : public iCallbacks
{
public:
    cViewer();
    ~cViewer();

    bool setInitialImagePath(const char* path);
    void initialize(GLFWwindow* window);
    enum class Property
    {
        Infobar,
        PixelInfo,
        Checkers,
        FitImage,
        Fullscreen,
        Border,
        Recursive,
        CenterWindow,
        AllValid
    };
    void SetProp(Property prop);
    void SetProp(unsigned char r, unsigned char g, unsigned char b);

    void render();
    bool isQuitRequested() const { return m_quitRequest; }

public:
    virtual void doProgress(int percent);

    void fnResize();
    void fnMouse(float x, float y);
    void fnMouseButtons(int button, int action, int mods);
    void fnKeyboard(int key, int scancode, int action, int mods);
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
    cVector<float> screenToImage(const cVector<float>& pos) const;
    cVector<float> calculateMousePosition(float x, float y) const;

private:
    GLFWwindow* m_window = nullptr;
    bool m_quitRequest = false;
    cVector<float> m_ratio;
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
    std::unique_ptr<CImageLoader> m_loader;
    std::unique_ptr<CInfoBar> m_infoBar;
    std::unique_ptr<CPixelInfo> m_pixelInfo;
    std::unique_ptr<CCheckerboard> m_checkerBoard;
    std::unique_ptr<CProgress> m_progress;
    std::unique_ptr<CImageBorder> m_border;
    std::unique_ptr<CSelection> m_selection;
};

