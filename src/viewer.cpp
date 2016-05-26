/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "viewer.h"
#include "quadimage.h"
#include "fileslist.h"
#include "imageloader.h"
#include "infobar.h"
#include "pixelinfo.h"
#include "checkerboard.h"
#include "progress.h"
#include "imageborder.h"
#include "selection.h"

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int DEF_WINDOW_W = 200;
const int DEF_WINDOW_H = 200;

cViewer::cViewer()
    : m_initialImageLoading(true)
    , m_scale(1.0f)
    , m_windowed(true)
    , m_center_window(false)
    , m_all_valid(false)
    , m_testFullscreen(false)
    , m_fitImage(false)
    , m_showBorder(false)
    , m_recursiveDir(false)
    , m_cursorVisible(true)
    , m_mouseLB(false)
    , m_mouseMB(false)
    , m_mouseRB(false)
    //, m_keyPressed(false)
    , m_angle(0)
{
    m_loader.reset(new CImageLoader(this));
    m_checkerBoard.reset(new CCheckerboard());
    m_infoBar.reset(new CInfoBar());
    m_pixelInfo.reset(new CPixelInfo());
    m_progress.reset(new CProgress());
    m_border.reset(new CImageBorder());
    m_selection.reset(new CSelection());
}

cViewer::~cViewer()
{
    deleteTextures();
}

bool cViewer::setInitialImagePath(const char* path)
{
    m_initialImageLoading = true;
    m_filesList.reset(new CFilesList(path, m_recursiveDir));
    m_filesList->setAllValid(m_all_valid);
    return m_filesList->GetName() != 0;
}

void cViewer::initialize(GLFWwindow* window)
{
    m_window = window;

    cRenderer::init();

    m_checkerBoard->Init(window);
    m_infoBar->Init(window);
    m_pixelInfo->Init();
    m_progress->Init();
    m_selection->Init();

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    updateFramebufferSize(width, height);
    updatePixelInfo({ (float)width, (float)height });
}

void cViewer::SetProp(Property prop)
{
    switch(prop)
    {
    case Property::Infobar:
        m_infoBar->Show(false);
        break;
    case Property::PixelInfo:
        m_pixelInfo->Show(true);
        break;
    case Property::Checkers:
        m_checkerBoard->Enable(false);
        break;
    case Property::FitImage:
        m_fitImage = true;
        break;
    case Property::Fullscreen:
        m_windowed = false;
        break;
    case Property::Border:
        m_showBorder = true;
        break;
    case Property::Recursive:
        m_recursiveDir = true;
        break;
    case Property::CenterWindow:
        m_center_window = true;
        break;
    case Property::AllValid:
        m_all_valid = true;
        break;
    }
}

void cViewer::SetProp(unsigned char r, unsigned char g, unsigned char b)
{
    m_checkerBoard->SetColor(r, g, b);
}

void cViewer::fnRender()
{
    //if(m_testFullscreen == true)
    //{
        //m_testFullscreen = false;

        ////printf("fullscreen desired, actual: %d x %d\n", width, height);
        //// if window can't be resized (due WM restriction or limitation) then set size to current window size
        //// useful in tiled WM
        //int a_width;
        //int a_height;
        ////glfwGetWindowSize(m_window, &a_width, &a_height);
        //glfwGetFramebufferSize(m_window, &a_width, &a_height);

        //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        //const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        ////printf("fullscreen desired: %d x %d, actual: %d x %d\n", mode->width, mode->height, a_width, a_height);
        //if(mode->width != a_width || mode->height != a_height)
        //{
            ////printf("can't set fullscreen mode. scr: %d x %d, win: %d x %d\n", mode->width, mode->height, width, height);
            //m_windowed = true;
            //centerWindow();
            //return;
        //}
    //}

    if(m_initialImageLoading == true)
    {
        m_initialImageLoading = false;
        loadImage(0);
    }

    m_checkerBoard->Render();

    //updateViewportSize();

    cRenderer::setGlobals(cVector<float>(m_camera.x, m_camera.y), m_angle, m_scale);

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

    if(m_showBorder)
    {
        m_border->Render(-half_w, -half_h, img_w, img_h, m_scale);
    }
    if(m_pixelInfo->IsVisible())
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

    m_infoBar->Render();
    m_pixelInfo->Render();
}

void cViewer::fnResize(int width, int height)
{
    updateFramebufferSize(width, height);
}

void cViewer::updateFramebufferSize(int width, int height)
{
    if(m_windowed)
    {
        storeWindowPositionSize(false, true);
    }

    width = (width + 1) & 0xfffffffe;
    height = (height + 1) & 0xfffffffe;
    m_viewport = cVector<float>(width, height);

    cRenderer::setWindowSize(m_viewport);

    m_pixelInfo->SetWindowSize(m_viewport);
    updateInfobar();

    int win_w, win_h;
    glfwGetWindowSize(m_window, &win_w, &win_h);

    m_ratio = { (float)width / win_w, (float)height / win_h };

    //printf("framebuffer: %d x %d -> window: %d x %d ratio: %.1f x %.1f\n", width, height, win_w, win_h, m_ratio.x, m_ratio.y);
}

void cViewer::fnMouse(float x, float y)
{
    x *= m_ratio.x;
    y *= m_ratio.y;

    const cVector<float> pointer_pos(x / m_scale, y / m_scale);
    const cVector<float> diff(m_lastMouse - pointer_pos);
    m_lastMouse = pointer_pos;
    if(m_mouseMB || m_mouseRB)
    {
        if(diff != cVector<float>())
        {
            shiftCamera(diff);
        }
    }

    if(m_pixelInfo->IsVisible())
    {
        const int cursor = m_selection->GetCursor();
        m_pixelInfo->SetCursor(cursor);

        const cVector<float> point = screenToImage(m_lastMouse);
        m_selection->MouseMove(point.x, point.y);

        updatePixelInfo(m_lastMouse);
    }
    else
    {
        showCursor(true);
    }
}

void cViewer::fnMouseButtons(int button, int action, int mods)
{
    (void)mods;

    switch(button)
    {
    case GLFW_MOUSE_BUTTON_4:
        updateScale(true);
        break;

    case GLFW_MOUSE_BUTTON_5:
        updateScale(false);
        break;

    case GLFW_MOUSE_BUTTON_LEFT:
        m_mouseLB = (action == GLFW_PRESS);
        if(m_pixelInfo->IsVisible())
        {
            double x, y;
            glfwGetCursorPos(m_window, &x, &y);
            const cVector<float> pos(x / m_scale, y / m_scale);
            const cVector<float> point = screenToImage(pos);
            m_selection->MouseButton(point.x, point.y, m_mouseLB);
            updatePixelInfo(pos);
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
        //cRenderer::disable(true);
        exit(0);
        break;

    case GLFW_KEY_I:
        m_infoBar->Show(!m_infoBar->Visible());
        //calculateScale();
        centerWindow();
        break;

    case GLFW_KEY_P:
        m_pixelInfo->Show(!m_pixelInfo->IsVisible());
        showCursor(!m_pixelInfo->IsVisible());
        break;

    case GLFW_KEY_S:
        m_fitImage = !m_fitImage;
        if(m_fitImage == false)
        {
            m_scale = 1.0f;
        }
        m_camera = cVector<float>();
        centerWindow();
        updateInfobar();
        m_selection->setScale(m_scale);
        break;

    case GLFW_KEY_SPACE:
        loadImage(1);
        break;

    case GLFW_KEY_BACKSPACE:
        loadImage(-1);
        break;

    case GLFW_KEY_DELETE:
        if(mods & GLFW_MOD_CONTROL)
        {
            m_filesList->RemoveFromDisk();
        }
        break;

    case GLFW_KEY_B:
        m_showBorder = !m_showBorder;
        break;

    case GLFW_KEY_EQUAL:
        updateScale(true);
        break;

    case GLFW_KEY_MINUS:
        updateScale(false);
        break;

    case GLFW_KEY_C:
        m_checkerBoard->Enable(!m_checkerBoard->IsEnabled());
        break;

    case GLFW_KEY_ENTER:
        m_windowed = !m_windowed;
        if(m_windowed)
        {
            centerWindow();
        }
        else
        {
            m_testFullscreen = true;

            storeWindowPositionSize(true, true);

            //glutFullScreen();
        }
        break;

    case GLFW_KEY_H:
    case GLFW_KEY_LEFT:
        keyLeft();
        break;

    case GLFW_KEY_RIGHT:
        keyRight();
        break;

    case GLFW_KEY_L:
        if(mods & GLFW_MOD_CONTROL)
        {
            m_angle += 90;
            m_angle %= 360;
            calculateScale();
        }
        else
        {
            keyRight();
        }
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
        if(mods & GLFW_MOD_CONTROL)
        {
            m_angle += 360 - 90;
            m_angle %= 360;
            calculateScale();
        }
        break;

    case GLFW_KEY_PAGE_UP:
        loadSubImage(-1);
        break;

    case GLFW_KEY_PAGE_DOWN:
        loadSubImage(1);
        break;

    default:
        //std::cout << key << std::endl;
        if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
        {
            m_scale = (float)(key - GLFW_KEY_0) + 1.0f;
            m_camera = cVector<float>();
            m_fitImage = false;
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

    cVector<float> half = (m_viewport / m_scale + cVector<float>(w, h)) * 0.5f;
    m_camera.x = std::max<float>(m_camera.x, -half.x);
    m_camera.x = std::min<float>(m_camera.x, half.x);
    m_camera.y = std::max<float>(m_camera.y, -half.y);
    m_camera.y = std::min<float>(m_camera.y, half.y);
}


void cViewer::calculateScale()
{
    if(m_fitImage && m_loader->isLoaded())
    {
        float w = static_cast<float>(m_loader->GetWidth());
        float h = static_cast<float>(m_loader->GetHeight());
        if(m_angle == 90 || m_angle == 270)
        {
            float t = w;
            w = h;
            h = t;
        }

        // scale only large images
        if(w >= m_viewport.x || h >= m_viewport.y)
        {
            float aspect = w / h;
            float new_w = 0;
            float new_h = 0;
            float dx = w / m_viewport.x;
            float dy = h / m_viewport.y;
            if(dx > dy)
            {
                if(w > m_viewport.x)
                {
                    new_w = m_viewport.x;
                    new_h = new_w / aspect;
                }
            }
            else
            {
                if(h > m_viewport.y)
                {
                    new_h = m_viewport.y;
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
    m_fitImage = false;

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

void cViewer::storeWindowPositionSize(bool position, bool size)
{
    if(position)
    {
        int x;
        int y;
        glfwGetWindowPos(m_window, &x, &y);
        m_prev_pos = cVector<float>(x, y);
    }

    if(size)
    {
        int width;
        int height;
        //glfwGetWindowSize(m_window, &width, &height);
        glfwGetFramebufferSize(m_window, &width, &height);
        m_prev_size = cVector<float>(width, height);
    }
}

void cViewer::centerWindow()
{
    //if(m_windowed)
    //{
        //int winw = m_prev_size.x;
        //int winh = m_prev_size.y;
        //int posx = m_prev_pos.x;
        //int posy = m_prev_pos.y;

        //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        //const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        //if(m_center_window)
        //{
            //// calculate window size
            //int w = m_loader->GetWidth();
            //int h = m_loader->GetHeight();
            //int imgw = std::max<int>(w + (m_showBorder ? m_border->GetBorderWidth() * 2 : 0), DEF_WINDOW_W);
            //int imgh = std::max<int>(h + (m_showBorder ? m_border->GetBorderWidth() * 2 : 0), DEF_WINDOW_H);
            //winw = std::min<int>(imgw, mode->width);
            //winh = std::min<int>(imgh, mode->height);

            //// calculate window position
            //posx = (mode->width - winw) / 2;
            //posy = (mode->height - winh) / 2;
        //}

        //glfwSetWindowPos(m_window, posx, posy);
        //glfwSetWindowSize(m_window, winw, winh);
        //printf("screen: %d x %d, window %d x %d, pos: %d, %d\n", mode->width, mode->height, winw, winh, posx, posy);

        //m_prev_pos = cVector<float>(posx, posy);
        //m_prev_size = cVector<float>(winw, winh);

        //calculateScale();
    //}
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

    const char* path = m_filesList->GetName(step);
    m_progress->Start();

    const bool result = m_loader->LoadImage(path, subImage);

    createTextures();

    m_selection->SetImageDimension(m_loader->GetWidth(), m_loader->GetHeight());
    updateInfobar();

    centerWindow();

    updatePixelInfo(m_lastMouse);

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

const cVector<float> cViewer::screenToImage(const cVector<float>& pos)
{
    const float w = m_loader->GetWidth();
    const float h = m_loader->GetHeight();

    return pos + m_camera - (m_viewport / m_scale - cVector<float>(w, h)) * 0.5f;
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

        pixelInfo.mouse = (pos - m_viewport / m_scale * 0.5f) * m_scale;
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

        std::cout << " " << width << " x " << height << ", ";

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
            const size_t quadsCount = cols * rows;
            std::cout << "textures: " << quadsCount << " (" << cols << " x " << rows << ") required" << std::endl;

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
    if(m_cursorVisible != show)
    {
        m_cursorVisible = show;
        glfwSetInputMode(m_window, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
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

