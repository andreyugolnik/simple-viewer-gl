/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "viewer.h"
#include "checkerboard.h"
#include "common/config.h"
#include "fileslist.h"
#include "imageborder.h"
#include "imageloader.h"
#include "infobar.h"
#include "overlayinfo.h"
#include "pixelinfo.h"
#include "progress.h"
#include "quadimage.h"
#include "selection.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <string>

namespace
{

    const int DEF_WINDOW_W = 300;
    const int DEF_WINDOW_H = 200;

}

cViewer::cViewer(sConfig* config)
    : m_config(config)
    , m_isWindowed(true)
    , m_mouseLB(false)
    , m_mouseMB(false)
    , m_mouseRB(false)
    , m_angle(0)
{
    m_image.reset(new cQuadImage());
    m_loader.reset(new cImageLoader(this));
    m_checkerBoard.reset(new cCheckerboard());
    m_infoBar.reset(new cInfoBar(config));
    m_pixelInfo.reset(new cPixelInfo());
    m_exifInfo.reset(new cOverlayInfo());
    m_progress.reset(new cProgress());
    m_border.reset(new cImageBorder());
    m_selection.reset(new cSelection());

    m_prevSize = { DEF_WINDOW_W, DEF_WINDOW_H };

    applyConfig();
}

cViewer::~cViewer()
{
    m_image->clear();
}

void cViewer::setInitialImagePath(const char* path)
{
    m_filesList.reset(new cFilesList(path, m_config->skipFilter, m_config->recursiveScan));
}

void cViewer::setWindow(GLFWwindow* window)
{
    m_windowModeChangeRequested = false;

    cRenderer::setWindow(window);

    m_checkerBoard->init();
    m_infoBar->init();
    m_pixelInfo->Init();
    m_exifInfo->init();
    m_progress->init();
    m_selection->Init();

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    m_ratio = { (float)frameWidth / width, (float)frameHeight / height };

    fnResize(width, height);
}

void cViewer::addPaths(const char** paths, int count)
{
    if (count != 0)
    {
        for (int i = 0; i < count; i++)
        {
            m_filesList->addFile(paths[i]);
        }

        m_filesList->sortList();
        m_filesList->locateFile(paths[0]);

        loadImage(0);
    }
}

void cViewer::applyConfig()
{
    m_checkerBoard->setColor(m_config->bgColor);
}

void cViewer::render()
{
    m_checkerBoard->render(!m_config->hideCheckboard);

    //updateViewportSize();

    const float scale = m_scale.getScale();

    cRenderer::setGlobals(m_camera, m_angle, scale);

    m_image->render();

    const float half_w = ceilf(m_image->getWidth() * 0.5f);
    const float half_h = ceilf(m_image->getHeight() * 0.5f);

    if (m_loader->isLoaded())
    {
        if (m_config->showImageBorder)
        {
            m_border->Render(-half_w, -half_h, m_image->getWidth(), m_image->getHeight(), scale);
        }
        if (m_config->showPixelInfo && m_angle == 0)
        {
            m_selection->Render(-half_w, -half_h);
        }
    }
    cRenderer::resetGlobals();

    if (m_config->showExif)
    {
        m_exifInfo->render();
    }

    //if(m_showBorder == true)
    //{
    //switch(m_angle)
    //{
    //case 0:
    //m_border->Render(m_camera_x, m_camera_y, img_w, img_h, scale);
    //break;
    //case 90:
    //m_border->Render(m_camera_x, m_camera_y, img_h, -img_w, scale);
    //break;
    //case 180:
    //m_border->Render(m_camera_x, m_camera_y, -img_w, -img_h, scale);
    //break;
    //case 270:
    //m_border->Render(m_camera_x, m_camera_y, -img_h, img_w, scale);
    //break;
    //}
    //}

    if (m_config->hideInfobar == false)
    {
        m_infoBar->render();
    }

    if (m_config->showPixelInfo && m_angle == 0)
    {
        m_pixelInfo->Render();
    }

    m_progress->render();

    glfwSwapBuffers(cRenderer::getWindow());
}

void cViewer::update()
{
    if (m_imagePrepared == true)
    {
        m_imagePrepared = false;

        auto& desc = m_loader->getDescription();
        m_image->setBuffer(desc.width, desc.height, desc.pitch
                           , desc.format, desc.bpp / 8
                           , desc.bitmap.data());

        if (m_loader->getMode() == cImageLoader::Mode::Image && m_config->keepScale == false)
        {
            m_scale.setScalePercent(100);
            m_angle = 0;
            m_camera = cVector<float>(0, 0);
            m_selection->SetImageDimension(desc.width, desc.height);
        }

        m_exifInfo->setExifList(desc.exifList);

        updateInfobar();
        centerWindow();
        enablePixelInfo(m_config->showPixelInfo);
    }

    if (isUploading())
    {
        const bool isDone = m_image->upload(m_config->mipmapTextureSize);
        m_progress->setProgress(0.5f + m_image->getProgress() * 0.5f);

        if (isDone)
        {
            m_progress->hide();

            auto& desc = m_loader->getDescription();
            m_animation = desc.isAnimation;
        }
    }
    else if (m_animation && m_subImageForced == false)
    {
        auto& desc = m_loader->getDescription();
        if (m_animationTime + desc.delay * 0.001f <= glfwGetTime())
        {
            m_animation = false;
            m_animationTime = glfwGetTime();
            loadSubImage(1);
        }
    }
}

bool cViewer::isUploading() const
{
    return m_image->isUploading();
}

void cViewer::fnResize(int width, int height)
{
    GLFWwindow* window = cRenderer::getWindow();

    if (m_isWindowed)
    {
        m_prevSize = { width, height };

        int x, y;
        glfwGetWindowPos(window, &x, &y);
        m_prevPos = { x, y };
    }

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

    cRenderer::setViewportSize({ (float)frameWidth, (float)frameHeight });

    m_pixelInfo->setRatio(m_ratio.y);
    m_exifInfo->setRatio(m_ratio.y);
    updatePixelInfo(m_lastMouse);

    m_infoBar->setRatio(m_ratio.y);
    updateInfobar();
}

void cViewer::fnPosition(int x, int y)
{
    m_prevPos = { x, y };
}

cVector<float> cViewer::calculateMousePosition(float x, float y) const
{
    x *= m_ratio.x;
    y *= m_ratio.y;

    const float scale = m_scale.getScale();
    return { x / scale, y / scale };
}

void cViewer::fnMouse(float x, float y)
{
    const cVector<float> pos(calculateMousePosition(x, y));

    if (m_mouseMB || m_mouseRB)
    {
        const cVector<float> diff(m_lastMouse - pos);
        m_lastMouse = pos;

        if (diff != cVector<float>())
        {
            shiftCamera(diff);
        }
    }

    if (m_config->showPixelInfo)
    {
        const int cursor = m_selection->GetCursor();
        m_pixelInfo->SetCursor(cursor);

        const cVector<float> point = screenToImage(pos);
        m_selection->MouseMove(point.x, point.y);

        updatePixelInfo(pos);
    }
    else
    {
        showCursor(true);
    }
}

void cViewer::fnMouseScroll(float x, float y)
{
    (void)x;
    if (m_config->wheelZoom)
    {
        updateScale(y > 0.0f);
    }
}

void cViewer::fnMouseButtons(int button, int action, int mods)
{
    (void)mods;

    updateMousePosition();

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        m_mouseLB = (action == GLFW_PRESS);
        {
            const cVector<float> point = screenToImage(m_lastMouse);
            m_selection->MouseButton(point.x, point.y, m_mouseLB);

            auto& rect = m_selection->GetRect();
            if (rect.IsSet() == false)
            {
                updatePixelInfo(m_lastMouse);
            }
        }
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        m_mouseMB = (action == GLFW_PRESS);
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        m_mouseRB = (action == GLFW_PRESS);
        break;
    }
}

void cViewer::fnKeyboard(int key, int /*scancode*/, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        return;
    }

    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(cRenderer::getWindow(), 1);
        break;

    case GLFW_KEY_I:
        m_config->hideInfobar = !m_config->hideInfobar;
        //calculateScale();
        centerWindow();
        break;

    case GLFW_KEY_E:
        m_config->showExif = !m_config->showExif;
        break;

    case GLFW_KEY_P:
        enablePixelInfo(!m_config->showPixelInfo);
        break;

    case GLFW_KEY_S:
        if (mods & GLFW_MOD_SHIFT)
        {
            m_config->keepScale = !m_config->keepScale;
        }
        else
        {
            m_config->fitImage = !m_config->fitImage;
            if (m_config->fitImage == false)
            {
                m_scale.setScalePercent(100);
            }
            m_camera = cVector<float>();
            centerWindow();
            updateInfobar();
            m_selection->setScale(m_scale.getScale());
        }
        break;

    case GLFW_KEY_SPACE:
        loadImage(1);
        break;

    case GLFW_KEY_BACKSPACE:
        loadImage(-1);
        break;

    case GLFW_KEY_DELETE:
        if (mods & GLFW_MOD_CONTROL)
        {
            m_filesList->removeFromDisk();
        }
        break;

    case GLFW_KEY_B:
        m_config->showImageBorder = !m_config->showImageBorder;
        break;

    case GLFW_KEY_EQUAL:
        updateScale(true);
        break;

    case GLFW_KEY_MINUS:
        updateScale(false);
        break;

    case GLFW_KEY_C:
        m_config->hideCheckboard = !m_config->hideCheckboard;
        break;

    case GLFW_KEY_ENTER:
        m_windowModeChangeRequested = true;
        break;

    case GLFW_KEY_H:
    case GLFW_KEY_LEFT:
        keyLeft();
        break;

    case GLFW_KEY_L:
    case GLFW_KEY_RIGHT:
        keyRight();
        break;

    case GLFW_KEY_K:
    case GLFW_KEY_UP:
        keyUp();
        break;

    case GLFW_KEY_J:
    case GLFW_KEY_DOWN:
        keyDown();
        break;

    case GLFW_KEY_R:
        if (mods & GLFW_MOD_SHIFT)
        {
            m_angle += 90;
            m_angle %= 360;
        }
        else
        {
            m_angle += 360 - 90;
            m_angle %= 360;
        }
        calculateScale();
        break;

    case GLFW_KEY_PAGE_UP:
        m_subImageForced = true;
        loadSubImage(-1);
        break;

    case GLFW_KEY_PAGE_DOWN:
        m_subImageForced = true;
        loadSubImage(1);
        break;

    default:
        if (key == GLFW_KEY_0)
        {
            m_scale.setScalePercent(1000);
            m_camera = { 0.0f, 0.0f };
            m_config->fitImage = false;
            centerWindow();
            updateInfobar();
            m_selection->setScale(m_scale.getScale());
        }
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
        {
            m_scale.setScalePercent((key - GLFW_KEY_0) * 100);
            m_camera = { 0.0f, 0.0f };
            m_config->fitImage = false;
            centerWindow();
            updateInfobar();
            m_selection->setScale(m_scale.getScale());
        }
        break;
    }
}

void cViewer::keyUp()
{
    shiftCamera(cVector<float>(0, -10 / m_scale.getScale()));
}

void cViewer::keyDown()
{
    shiftCamera(cVector<float>(0, 10 / m_scale.getScale()));
}

void cViewer::keyLeft()
{
    shiftCamera(cVector<float>(-10 / m_scale.getScale(), 0));
}

void cViewer::keyRight()
{
    shiftCamera(cVector<float>(10 / m_scale.getScale(), 0));
}

void cViewer::shiftCamera(const cVector<float>& delta)
{
    m_camera += delta;

    const auto& viewport = cRenderer::getViewportSize();
    cVector<float> half = (viewport / m_scale.getScale() + cVector<float>(m_image->getWidth(), m_image->getHeight())) * 0.5f;
    m_camera.x = std::max<float>(m_camera.x, -half.x);
    m_camera.x = std::min<float>(m_camera.x, half.x);
    m_camera.y = std::max<float>(m_camera.y, -half.y);
    m_camera.y = std::min<float>(m_camera.y, half.y);
}

void cViewer::calculateScale()
{
    if (m_config->fitImage && m_loader->isLoaded())
    {
        float w = static_cast<float>(m_image->getWidth());
        float h = static_cast<float>(m_image->getHeight());
        if (m_angle == 90 || m_angle == 270)
        {
            std::swap(w, h);
        }

        // scale only large images
        const auto& viewport = cRenderer::getViewportSize();
        if (w >= viewport.x || h >= viewport.y)
        {
            float aspect = w / h;
            float new_w = 0.0f;
            float new_h = 0.0f;
            float dx = w / viewport.x;
            float dy = h / viewport.y;
            if (dx > dy)
            {
                if (w > viewport.x)
                {
                    new_w = viewport.x;
                    new_h = new_w / aspect;
                }
            }
            else
            {
                if (h > viewport.y)
                {
                    new_h = viewport.y;
                    new_w = new_h * aspect;
                }
            }
            if (new_w != 0.0f && new_h != 0.0f)
            {
                //m_scale = static_cast<float>((angle == 0 || angle == 180) ? new_w : new_h) / w;
                m_scale.setScale(new_w / w);
            }
        }
        else
        {
            m_scale.setScalePercent(100);
        }
        m_selection->setScale(m_scale.getScale());
    }

    updateFiltering();
}

bool FixeScale(int& scale, int step)
{
    const int oldScale = scale;
    scale /= step;
    scale *= step;
    return oldScale != scale;
}

// TODO update m_camera_x / m_camera_y according current mouse position
void cViewer::updateScale(bool up)
{
    m_config->fitImage = false;

    int scale = m_scale.getScalePercent();

    if (up == true)
    {
        const int step = scale >= 100 ? 25 : (scale >= 50 ? 10 : (scale >= 30 ? 5 : 1));
        FixeScale(scale, step);
        scale += step;
    }
    else
    {
        const int step = scale > 100 ? 25 : (scale > 50 ? 10 : (scale > 30 ? 5 : 1));
        if (FixeScale(scale, step) == false && scale > step)
        {
            scale -= step;
        }
    }
    m_scale.setScalePercent(scale);
    m_selection->setScale(m_scale.getScale());

    updateFiltering();
    updateInfobar();
}

void cViewer::updateFiltering()
{
    const int scale = m_scale.getScalePercent();
    if (scale >= 100 && scale % 100 == 0)
    {
        m_image->useFilter(false);
    }
    else
    {
        m_image->useFilter(true);
    }
}

void cViewer::centerWindow()
{
    GLFWwindow* window = cRenderer::getWindow();

    if (m_isWindowed)
    {
        if (m_config->centerWindow)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // calculate window size
            int imgw = m_image->getWidth() + (m_config->showImageBorder ? m_border->GetBorderWidth() * 2 : 0);
            int imgh = m_image->getHeight() + (m_config->showImageBorder ? m_border->GetBorderWidth() * 2 : 0);
            imgw = std::max<int>(imgw, DEF_WINDOW_W * m_ratio.x);
            imgh = std::max<int>(imgh, DEF_WINDOW_H * m_ratio.y);

            const int width = std::min<int>(imgw / m_ratio.x, mode->width / m_ratio.x);
            const int height = std::min<int>(imgh / m_ratio.y, mode->height / m_ratio.y);

            // calculate window position
            const int x = (mode->width - width) / 2;
            const int y = (mode->height - height) / 2;

            glfwSetWindowSize(window, width, height);
            glfwSetWindowPos(window, x, y);

            m_prevSize = { width, height };
            m_prevPos = { x, y };
        }

        calculateScale();
    }
}

void cViewer::loadImage(int step)
{
    m_subImageForced = false;
    m_animation = false;
    const char* file = m_filesList->getName(step);
    m_loader->loadImage(file);
}

void cViewer::loadSubImage(int subStep)
{
    assert(subStep == -1 || subStep == 1);

    m_animation = false;
    auto& desc = m_loader->getDescription();
    const unsigned next = (desc.current + desc.images + subStep) % desc.images;
    if (desc.current != next)
    {
        m_loader->loadSubImage(next);
    }
}

void cViewer::updateInfobar()
{
    calculateScale();

    cInfoBar::sInfo s;
    s.path        = m_filesList->getName();
    s.scale       = m_scale.getScale();
    s.index       = m_filesList->getIndex();
    s.files_count = m_filesList->getCount();

    if (m_loader->isLoaded())
    {
        auto& desc = m_loader->getDescription();
        s.width       = desc.width;
        s.height      = desc.height;
        s.bpp         = desc.bppImage;
        s.images      = desc.images;
        s.current     = desc.current;
        s.file_size   = desc.size;
        s.mem_size    = desc.bitmap.size();
        s.type        = m_loader->getImageType();
    }
    else
    {
        s.type = "unknown";
    }
    m_infoBar->setInfo(s);
}

cVector<float> cViewer::screenToImage(const cVector<float>& pos) const
{
    const auto& viewport = cRenderer::getViewportSize();
    return pos + m_camera - (viewport / m_scale.getScale() - cVector<float>(m_image->getWidth(), m_image->getHeight())) * 0.5f;
}

void cViewer::updatePixelInfo(const cVector<float>& pos)
{
    sPixelInfo pixelInfo;

    const cVector<float> point = screenToImage(pos);

    pixelInfo.mouse = pos * m_scale.getScale();
    pixelInfo.point = point;

    if (m_loader->isLoaded())
    {
        auto& desc = m_loader->getDescription();
        pixelInfo.bpp = desc.bpp;

        const int x = (int)point.x;
        const int y = (int)point.y;

        if (x >= 0 && y >= 0 && (unsigned)x <= m_image->getWidth() && (unsigned)y <= m_image->getHeight())
        {
            const auto idx = (size_t)(x * desc.bpp / 8 + y * desc.pitch);
            const auto color = desc.bitmap.data() + idx;

            if (desc.bpp == 24 || desc.bpp == 32)
            {
                const bool bgrx = desc.format == GL_BGRA || desc.format == GL_BGR;
                pixelInfo.r = color[bgrx ? 0 : 2];
                pixelInfo.g = color[1];
                pixelInfo.b = color[bgrx ? 2 : 0];
                pixelInfo.a = color[3];
            }
            else if (desc.bpp == 16)
            {
                const float norm = 255 / 63;
                pixelInfo.r = (color[0] >> 3) * norm;
                pixelInfo.g = (((color[0] & 0x07) << 3) | ((color[1] & 0xe0) >> 5)) * norm;
                pixelInfo.b = (color[1] >> 3) * norm;
            }
            else if (desc.bpp == 8)
            {
                pixelInfo.r = color[0];
                pixelInfo.g = color[0];
                pixelInfo.b = color[0];
            }
        }

        pixelInfo.imgWidth = m_image->getWidth();
        pixelInfo.imgHeight = m_image->getHeight();
        pixelInfo.rc = m_selection->GetRect();
    }

    m_pixelInfo->setPixelInfo(pixelInfo);
}

void cViewer::showCursor(bool show)
{
    GLFWwindow* window = cRenderer::getWindow();
    glfwSetInputMode(window, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void cViewer::startLoading()
{
    auto& desc = m_loader->getDescription();
    if (desc.isAnimation == false)
    {
        m_progress->show();
    }
    m_imagePrepared = false;
}

void cViewer::doProgress(float progress)
{
    m_progress->setProgress(progress * 0.5f);
}

void cViewer::endLoading()
{
    m_imagePrepared = true;
}

void cViewer::updateMousePosition()
{
    GLFWwindow* window = cRenderer::getWindow();

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    m_lastMouse = calculateMousePosition(x, y);
}

void cViewer::enablePixelInfo(bool show)
{
    if (show)
    {
        updateMousePosition();
        updatePixelInfo(m_lastMouse);
    }
    m_config->showPixelInfo = show;
    showCursor(!show);
}
