/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "common/callbacks.h"
#include "common/scale.h"
#include "math/vector.h"

#include <vector>
#include <memory>

class cCheckerboard;
class cFilesList;
class cImageBorder;
class cImageLoader;
class cInfoBar;
class cPixelInfo;
class cProgress;
class cSelection;
class cQuadImage;
struct GLFWwindow;
struct sConfig;

class cViewer final : public iCallbacks
{
public:
    explicit cViewer(sConfig* config);
    ~cViewer();

    void setInitialImagePath(const char* path);
    void setWindow(GLFWwindow* window);

    void addPaths(const char** paths, int count);

    void render();
    void update();
    bool isUploading() const;

    bool isWindowModeRequested() const
    {
        return m_windowModeChangeRequested;
    }

    bool isWindowed() const
    {
        return m_isWindowed;
    }
    void setWindowed(bool windowed)
    {
        m_isWindowed = windowed;
    }

    const cVector<int>& getWindowPosition() const
    {
        return m_prevPos;
    }
    const cVector<int>& getWindowSize() const
    {
        return m_prevSize;
    }

public:
    virtual void startLoading() override;
    virtual void doProgress(float progress) override;
    virtual void endLoading() override;

    void fnResize(int width, int height);
    void centerWindow();
    void fnPosition(int x, int y);
    void fnMouse(float x, float y);
    void fnMouseScroll(float x, float y);
    void fnMouseButtons(int button, int action, int mods);
    void fnKeyboard(int key, int scancode, int action, int mods);
    void showCursor(bool show);
    void loadImage(int step);

private:
    void applyConfig();
    void loadSubImage(int subStep);
    void calculateScale();
    void updateScale(bool up);
    void updateFiltering();
    void updateInfobar();
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
    sConfig* m_config;
    cVector<float> m_ratio;
    bool m_imagePrepared = false;
    cScale m_scale;
    bool m_isWindowed;
    //bool m_centerWindow;
    //bool m_allValid;
    //bool m_wheelZoom;
    bool m_windowModeChangeRequested;
    //bool m_fitImage;
    //bool m_showBorder;
    //bool m_recursiveDir;
    bool m_mouseLB, m_mouseMB, m_mouseRB;
    cVector<float> m_lastMouse;
    cVector<float> m_camera;
    cVector<int> m_prevPos;
    cVector<int> m_prevSize;
    int m_angle;

    bool m_subImageForced = false;
    bool m_animation = false;
    float m_animationTime = 0.0f;

    std::unique_ptr<cQuadImage> m_image;
    std::unique_ptr<cFilesList> m_filesList;
    std::unique_ptr<cProgress> m_progress;
    std::unique_ptr<cImageLoader> m_loader;
    std::unique_ptr<cInfoBar> m_infoBar;
    std::unique_ptr<cPixelInfo> m_pixelInfo;
    std::unique_ptr<cCheckerboard> m_checkerBoard;
    std::unique_ptr<cImageBorder> m_border;
    std::unique_ptr<cSelection> m_selection;
};
