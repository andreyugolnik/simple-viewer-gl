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
#include "config.h"
#include "fileslist.h"
#include "imageborder.h"
#include "imageloader.h"
#include "infobar.h"
#include "pixelinfo.h"
#include "progress.h"
#include "quadimage.h"
#include "selection.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <string>

const int DEF_WINDOW_W = 200;
const int DEF_WINDOW_H = 200;

cViewer::cViewer(sConfig* config)
    : m_config(config)
    , m_initialImageLoading(true)
    , m_scale(1.0f)
    , m_isWindowed(true)
    , m_mouseLB(false)
    , m_mouseMB(false)
    , m_mouseRB(false)
    , m_angle(0)
{
    m_loader.reset(new CImageLoader(this));
    m_checkerBoard.reset(new CCheckerboard());
    m_infoBar.reset(new CInfoBar());
    m_pixelInfo.reset(new CPixelInfo());
    m_progress.reset(new CProgress());
    m_border.reset(new CImageBorder());
    m_selection.reset(new CSelection());

    m_prevSize = { DEF_WINDOW_W, DEF_WINDOW_H };

    applyConfig();
}

cViewer::~cViewer()
{
    deleteTextures();
}

bool cViewer::setInitialImagePath(const char* path)
{
    m_initialImageLoading = true;
    m_filesList.reset(new CFilesList(path, m_config->recursiveScan));
    m_filesList->setAllValid(m_config->skipFilter);
    return m_filesList->GetName() != 0;
}

void cViewer::setWindow(GLFWwindow* window)
{
    m_windowModeChangeRequested = false;

    cRenderer::setWindow(window);

    m_checkerBoard->init();
    m_infoBar->init();
    m_pixelInfo->Init();
    m_progress->Init();
    m_selection->Init();

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    m_ratio = { (float)frameWidth / width, (float)frameHeight / height };

    fnResize(width, height);

    if(m_initialImageLoading == true)
    {
        m_initialImageLoading = false;
        loadImage(0, 0);
    }
}

void cViewer::addPaths(const char** paths, int count)
{
    (void)count;

    m_filesList.reset(new CFilesList(paths[0], m_config->recursiveScan));
    m_filesList->setAllValid(m_config->skipFilter);

    loadImage(0, 0);
}

void cViewer::applyConfig()
{
    const sColor& c = m_config->color;
    m_checkerBoard->setColor(c.r, c.g, c.b);
}

void cViewer::render()
{
    m_checkerBoard->render(!m_config->hideCheckboard);

    //updateViewportSize();

    cRenderer::setGlobals(m_camera, m_angle, m_scale);

    const unsigned img_w = m_loader->GetWidth();
    const unsigned img_h = m_loader->GetHeight();

    const float half_w = ceilf(img_w * 0.5f);
    const float half_h = ceilf(img_h * 0.5f);
    for(size_t i = 0, size = m_quads.size(); i < size; i++)
    {
        CQuadImage* quad = m_quads[i];
        const float x = quad->GetCol() * quad->GetTexWidth() - half_w;
        const float y = quad->GetRow() * quad->GetTexHeight() - half_h;

        quad->Render(x, y);
    }

    if(m_config->showImageBorder)
    {
        m_border->Render(-half_w, -half_h, img_w, img_h, m_scale);
    }
    if(m_config->showPixelInfo && m_angle == 0)
    {
        m_selection->Render(-half_w, -half_h);
    }
    cRenderer::resetGlobals();

    //if(m_showBorder == true)
    //{
        //switch(m_angle)
        //{
        //case 0:
            //m_border->Render(m_camera_x, m_camera_y, img_w, img_h, m_scale);
            //break;
        //case 90:
            //m_border->Render(m_camera_x, m_camera_y, img_h, -img_w, m_scale);
            //break;
        //case 180:
            //m_border->Render(m_camera_x, m_camera_y, -img_w, -img_h, m_scale);
            //break;
        //case 270:
            //m_border->Render(m_camera_x, m_camera_y, -img_h, img_w, m_scale);
            //break;
        //}
    //}

    if(m_config->hideInfobar == false)
    {
        m_infoBar->Render();
    }

    if(m_config->showPixelInfo && m_angle == 0)
    {
        m_pixelInfo->Render();
    }

    glfwSwapBuffers(cRenderer::getWindow());
}

void cViewer::fnResize(int width, int height)
{
    GLFWwindow* window = cRenderer::getWindow();

    if(m_isWindowed)
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

    return { x / m_scale, y / m_scale };
}

void cViewer::fnMouse(float x, float y)
{
    const cVector<float> pos(calculateMousePosition(x, y));

    if(m_mouseMB || m_mouseRB)
    {
        const cVector<float> diff(m_lastMouse - pos);
        m_lastMouse = pos;

        if(diff != cVector<float>())
        {
            shiftCamera(diff);
        }
    }

    if(m_config->showPixelInfo)
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
    if(m_config->wheelZoom)
    {
        updateScale(y > 0.0f);
    }
}

void cViewer::fnMouseButtons(int button, int action, int mods)
{
    (void)mods;

    updateMousePosition();

    switch(button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        m_mouseLB = (action == GLFW_PRESS);
        {
            const cVector<float> point = screenToImage(m_lastMouse);
            m_selection->MouseButton(point.x, point.y, m_mouseLB);
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

void cViewer::fnKeyboard(int key, int scancode, int action, int mods)
{
    (void)scancode;
    if(action != GLFW_PRESS)
    {
        return;
    }

    switch(key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(cRenderer::getWindow(), 1);
        break;

    case GLFW_KEY_I:
        m_config->hideInfobar = !m_config->hideInfobar;
        //calculateScale();
        centerWindow();
        break;

    case GLFW_KEY_P:
        enablePixelInfo(!m_config->showPixelInfo);
        break;

    case GLFW_KEY_S:
        m_config->fitImage = !m_config->fitImage;
        if(m_config->fitImage == false)
        {
            m_scale = 1.0f;
        }
        m_camera = cVector<float>();
        centerWindow();
        updateInfobar();
        m_selection->setScale(m_scale);
        break;

    case GLFW_KEY_SPACE:
        loadImage(1, 0);
        break;

    case GLFW_KEY_BACKSPACE:
        loadImage(-1, 0);
        break;

    case GLFW_KEY_DELETE:
        if(mods & GLFW_MOD_CONTROL)
        {
            m_filesList->RemoveFromDisk();
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
        m_isWindowed = !m_isWindowed;
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
        if(mods & GLFW_MOD_SHIFT)
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
        loadSubImage(-1);
        break;

    case GLFW_KEY_PAGE_DOWN:
        loadSubImage(1);
        break;

    default:
        if(key == GLFW_KEY_0)
        {
            m_scale = 10.0f;
            m_camera = { 0.0f, 0.0f };
            m_config->fitImage = false;
            centerWindow();
            updateInfobar();
            m_selection->setScale(m_scale);
        }
        else if(key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
        {
            m_scale = (float)(key - GLFW_KEY_0);
            m_camera = { 0.0f, 0.0f };
            m_config->fitImage = false;
            centerWindow();
            updateInfobar();
            m_selection->setScale(m_scale);
        }
        break;
    }
}

void cViewer::keyUp()
{
    shiftCamera(cVector<float>(0, -10));
}

void cViewer::keyDown()
{
    shiftCamera(cVector<float>(0, 10));
}

void cViewer::keyLeft()
{
    shiftCamera(cVector<float>(-10, 0));
}

void cViewer::keyRight()
{
    shiftCamera(cVector<float>(10, 0));
}

void cViewer::shiftCamera(const cVector<float>& delta)
{
    m_camera += delta;

    const unsigned w = m_loader->GetWidth();
    const unsigned h = m_loader->GetHeight();

    const auto& viewport = cRenderer::getViewportSize();
    cVector<float> half = (viewport / m_scale + cVector<float>(w, h)) * 0.5f;
    m_camera.x = std::max<float>(m_camera.x, -half.x);
    m_camera.x = std::min<float>(m_camera.x, half.x);
    m_camera.y = std::max<float>(m_camera.y, -half.y);
    m_camera.y = std::min<float>(m_camera.y, half.y);
}


void cViewer::calculateScale()
{
    if(m_config->fitImage && m_loader->isLoaded())
    {
        float w = static_cast<float>(m_loader->GetWidth());
        float h = static_cast<float>(m_loader->GetHeight());
        if(m_angle == 90 || m_angle == 270)
        {
            std::swap(w, h);
        }

        // scale only large images
        const auto& viewport = cRenderer::getViewportSize();
        if(w >= viewport.x || h >= viewport.y)
        {
            float aspect = w / h;
            float new_w = 0;
            float new_h = 0;
            float dx = w / viewport.x;
            float dy = h / viewport.y;
            if(dx > dy)
            {
                if(w > viewport.x)
                {
                    new_w = viewport.x;
                    new_h = new_w / aspect;
                }
            }
            else
            {
                if(h > viewport.y)
                {
                    new_h = viewport.y;
                    new_w = new_h * aspect;
                }
            }
            if(new_w != 0 && new_h != 0)
            {
                //m_scale = static_cast<float>((angle == 0 || angle == 180) ? new_w : new_h) / w;
                m_scale = new_w / w;
            }
        }
        else
        {
            m_scale = 1.0f;
        }
        m_selection->setScale(m_scale);
    }

    updateFiltering();
}

// TODO update m_camera_x / m_camera_y according current mouse position
void cViewer::updateScale(bool up)
{
    m_config->fitImage = false;

    const int step = 25;
    int scale = std::max<int>(step, ceilf(m_scale * 100.0f));
    scale /= 25;
    scale *= 25;

    if(up == true)
    {
        scale += step;
    }
    else
    {
        if(scale > step)
        {
            scale -= step;
        }
    }
    m_scale = scale / 100.0f;
    m_selection->setScale(m_scale);

    updateFiltering();
    updateInfobar();
}

void cViewer::updateFiltering()
{
    const int scale = (int)(m_scale * 100);
    if(scale % 100 == 0 && m_scale >= 1.0f)
    {
        for(size_t i = 0, size = m_quads.size(); i < size; i++)
        {
            m_quads[i]->useFilter(false);
        }
    }
    else
    {
        for(size_t i = 0, size = m_quads.size(); i < size; i++)
        {
            m_quads[i]->useFilter(true);
        }
    }
}

void cViewer::centerWindow()
{
    GLFWwindow* window = cRenderer::getWindow();

    if(m_isWindowed)
    {
        if(m_config->centerWindow)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // calculate window size
            int imgw = m_loader->GetWidth() + (m_config->showImageBorder ? m_border->GetBorderWidth() * 2 : 0);
            int imgh = m_loader->GetHeight() + (m_config->showImageBorder ? m_border->GetBorderWidth() * 2 : 0);
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

bool cViewer::loadSubImage(int subStep)
{
    const int subCount = (int)m_loader->GetSubCount();
    int subImage = (int)m_loader->GetSub() + subStep;

    if(subImage < 0)
    {
        subImage = subCount - 1;
    }
    else if(subImage >= subCount)
    {
        subImage = 0;
    }

    return loadImage(0, subImage);
}

bool cViewer::loadImage(int step, int subImage)
{
    if(step != 0)
    {
        m_scale = 1;
        m_angle = 0;
        m_camera = cVector<float>(0, 0);
        m_filesList->ParseDir();
    }

    const char* file = m_filesList->GetName(step);

    m_progress->Start();

    const bool result = m_loader->LoadImage(file, subImage);

    createTextures();

    m_selection->SetImageDimension(m_loader->GetWidth(), m_loader->GetHeight());
    updateInfobar();

    centerWindow();

    enablePixelInfo(m_config->showPixelInfo);

    //m_loader->FreeMemory();

    return result;
}

void cViewer::updateInfobar()
{
    calculateScale();

    sInfoBar s;
    s.path        = m_filesList->GetName(0);
    s.scale       = m_scale;
    s.index       = m_filesList->GetIndex();
    s.files_count = m_filesList->GetCount();

    if(m_loader->isLoaded())
    {
        s.width       = m_loader->GetWidth();
        s.height      = m_loader->GetHeight();
        s.bpp         = m_loader->GetImageBpp();
        s.sub_image   = m_loader->GetSub();
        s.sub_count   = m_loader->GetSubCount();
        s.file_size   = m_loader->GetSize();
        s.mem_size    = m_loader->GetSizeMem();
        s.type        = m_loader->getImageType();
    }
    else
    {
        s.type        = "unknown";
    }
    m_infoBar->Update(s);
}

cVector<float> cViewer::screenToImage(const cVector<float>& pos) const
{
    const float w = m_loader->GetWidth();
    const float h = m_loader->GetHeight();

    const auto& viewport = cRenderer::getViewportSize();
    return pos + m_camera - (viewport / m_scale - cVector<float>(w, h)) * 0.5f;
}

void cViewer::updatePixelInfo(const cVector<float>& pos)
{
    if(m_loader->isLoaded())
    {
        const int w = m_loader->GetWidth();
        const int h = m_loader->GetHeight();

        const cVector<float> point = screenToImage(pos);
        const int x = (int)point.x;
        const int y = (int)point.y;

        sPixelInfo pixelInfo;

        // TODO check pixel format (RGB or BGR)
        if(x >= 0 && y >= 0 && x <= w && y <= h)
        {
            const int bpp = m_loader->GetBpp();
            const int pitch = m_loader->GetPitch();
            const size_t idx = (size_t)(x * bpp / 8 + y * pitch);
            const unsigned char* color = m_loader->GetBitmap() + idx;
            pixelInfo.r = color[0];
            pixelInfo.g = color[1];
            pixelInfo.b = color[2];
            pixelInfo.a = bpp == 32 ? color[3] : 255;
        }

        pixelInfo.mouse = pos * m_scale;
        pixelInfo.point = point;
        pixelInfo.img_w = w;
        pixelInfo.img_h = h;
        pixelInfo.rc    = m_selection->GetRect();
        m_pixelInfo->setPixelInfo(pixelInfo);
    }
}

void cViewer::createTextures()
{
    unsigned char* bitmap = m_loader->GetBitmap();
    if(bitmap)
    {
        const int width   = m_loader->GetWidth();
        const int height  = m_loader->GetHeight();
        const int format  = m_loader->GetBitmapFormat();
        const int pitch   = m_loader->GetPitch();
        const int bytesPP = (m_loader->GetBpp() / 8);

        printf(" %d x %d, ", width, height);

        int texW, texH;
        cRenderer::calculateTextureSize(&texW, &texH, width, height);
        //if(texW > 0 && texH > 0)
        {
            // texture pitch should be multiple by 4
            const unsigned texPitch = (unsigned)ceilf(texW * bytesPP / 4.0f) * 4;
            //const unsigned line = texW * bytesPP;
            //const unsigned texPitch = line + (line % 4) * 4;

            const int cols = (int)ceilf((float)width / texW);
            const int rows = (int)ceilf((float)height / texH);
            printf("textures: %d (%d x %d) required\n", cols * rows, cols, rows);

            deleteTextures();

            unsigned char* buffer = new unsigned char[texPitch * texH];

            unsigned idx = 0;
            int height2 = height;
            for(int row = 0; row < rows; row++)
            {
                int width2 = width;
                unsigned h = (height2 > texH ? texH : height2);
                for(int col = 0; col < cols; col++)
                {
                    unsigned w = (width2 > texW ? texW : width2);
                    width2 -= w;

                    unsigned dx = col * texPitch;
                    unsigned dy = row * texH;
                    unsigned count = w * bytesPP;
                    for(unsigned line = 0; line < h; line++)
                    {
                        const unsigned src = dx + (dy + line) * pitch;
                        const unsigned dst = line * texPitch;
                        memcpy(&buffer[dst], &bitmap[src], count);
                    }

                    CQuadImage* quad = new CQuadImage(texW, texH, buffer, format);
                    quad->SetCell(col, row);
                    quad->SetSpriteSize(w, h);
                    quad->useFilter(false);

                    m_quads.push_back(quad);

                    idx++;
                }
                height2 -= h;
            }

            delete[] buffer;
        }
    }
}

void cViewer::showCursor(bool show)
{
    GLFWwindow* window = cRenderer::getWindow();
    glfwSetInputMode(window, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void cViewer::deleteTextures()
{
    for(size_t i = 0, size = m_quads.size(); i < size; i++)
    {
        delete m_quads[i];
    }
    m_quads.clear();
}

void cViewer::doProgress(int percent)
{
    m_progress->Render(percent);
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
    if(show)
    {
        updateMousePosition();
        updatePixelInfo(m_lastMouse);
    }
    m_config->showPixelInfo = show;
    showCursor(!show);
}

