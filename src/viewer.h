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
    void setWindow(GLFWwindow* window);

    void addPaths(const char** paths, int count);

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
        AllValid,
        WheelZoom,
    };
    void SetProp(Property prop);
    void SetProp(unsigned char r, unsigned char g, unsigned char b);

    void render();

    bool isWindowModeRequested() const { return m_windowModeChangeRequested; }
    bool isWindowed() const { return m_isWindowed; }
    const cVector<int>& getWindowPosition() const { return m_prevPos; }
    const cVector<int>& getWindowSize() const { return m_prevSize; }

public:
    virtual void doProgress(int percent);

    void fnResize(int width, int height);
    void centerWindow();
    void fnPosition(int x, int y);
    void fnMouse(float x, float y);
    void fnMouseScroll(float x, float y);
    void fnMouseButtons(int button, int action, int mods);
    void fnKeyboard(int key, int scancode, int action, int mods);
    void showCursor(bool show);

private:
    bool loadSubImage(int subStep);
    bool loadImage(int step, int subImage);
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
    void updateMousePosition();
    void enablePixelInfo(bool show);

private:
    cVector<float> m_ratio;
    bool m_initialImageLoading;
    float m_scale;
    bool m_isWindowed;
    bool m_centerWindow;
    bool m_allValid;
    bool m_wheelZoom;
    bool m_windowModeChangeRequested;
    bool m_fitImage;
    bool m_showBorder;
    bool m_recursiveDir;
    bool m_mouseLB, m_mouseMB, m_mouseRB;
    cVector<float> m_lastMouse;
    cVector<float> m_camera;
    cVector<int> m_prevPos;
    cVector<int> m_prevSize;
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

