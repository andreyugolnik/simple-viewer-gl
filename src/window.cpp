/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "window.h"
#include "quadimage.h"
#include "fileslist.h"
#include "imageloader.h"
#include "infobar.h"
#include "pixelinfo.h"
#include "checkerboard.h"
#include "notavailable.h"
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

#if defined(__APPLE__)
    const int key_delete = 8;
    const int key_backspace = 127;
#else
    const int key_delete = 127;
    const int key_backspace = 8;
#endif

CWindow::CWindow()
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
    m_lastMouse = cVector<float>(-1, -1);
    m_prev_size = cVector<float>(DEF_WINDOW_W, DEF_WINDOW_H);

    m_imageList.reset(new CImageLoader(this));
    m_checkerBoard.reset(new CCheckerboard());
    m_na.reset(new CNotAvailable());
    m_infoBar.reset(new CInfoBar());
    m_pixelInfo.reset(new CPixelInfo());
    m_progress.reset(new CProgress());
    m_border.reset(new CImageBorder());
    m_selection.reset(new CSelection());
}

CWindow::~CWindow()
{
    deleteTextures();
}

bool CWindow::setInitialImagePath(const char* path)
{
    m_initialImageLoading = true;
    m_filesList.reset(new CFilesList(path, m_recursiveDir));
    m_filesList->setAllValid(m_all_valid);
    return m_filesList->GetName() != 0;
}

void CWindow::run()
{
    cRenderer::init();

    m_checkerBoard->Init();
    m_na->Init();
    m_infoBar->Init();
    m_pixelInfo->Init();
    updatePixelInfo(cVector<float>(DEF_WINDOW_W, DEF_WINDOW_H));
    m_progress->Init();
    m_selection->Init();
}

void CWindow::SetProp(Property prop)
{
    switch(prop)
    {
    case PROP_INFOBAR:
        m_infoBar->Show(false);
        break;
    case PROP_PIXELINFO:
        m_pixelInfo->Show(true);
        break;
    case PROP_CHECKERS:
        m_checkerBoard->Enable(false);
        break;
    case PROP_FITIMAGE:
        m_fitImage = true;
        break;
    case PROP_FULLSCREEN:
        m_windowed = false;
        break;
    case PROP_BORDER:
        m_showBorder = true;
        break;
    case PROP_RECURSIVE:
        m_recursiveDir = true;
        break;
    case PROP_CENTER_WINDOW:
        m_center_window = true;
        break;
    case PROP_ALL_VALID:
        m_all_valid = true;
        break;
    }
}

void CWindow::SetProp(unsigned char r, unsigned char g, unsigned char b)
{
    m_checkerBoard->SetColor(r, g, b);
}

void CWindow::fnRender()
{
    if(m_testFullscreen == true)
    {
        m_testFullscreen = false;

        //printf("fullscreen desired, actual: %d x %d\n", width, height);
        // if window can't be resized (due WM restriction or limitation) then set size to current window size
        // useful in tiled WM
        const int a_width = glutGet(GLUT_WINDOW_WIDTH);
        const int a_height = glutGet(GLUT_WINDOW_HEIGHT);
        const int scrw = glutGet(GLUT_SCREEN_WIDTH);
        const int scrh = glutGet(GLUT_SCREEN_HEIGHT);
        //printf("fullscreen desired: %d x %d, actual: %d x %d\n", scrw, scrh, a_width, a_height);
        if(scrw != a_width || scrh != a_height)
        {
            //printf("can't set fullscreen mode. scr: %d x %d, win: %d x %d\n", scrw, scrh, width, height);
            m_windowed = true;
            centerWindow();
            return;
        }
    }

    if(m_initialImageLoading == true)
    {
        m_initialImageLoading = false;
        loadImage(0);
    }

    m_checkerBoard->Render();

    //updateViewportSize();

    if(m_na->Render() == false)
    {
        cRenderer::setGlobals(cVector<float>(m_camera.x, m_camera.y), m_angle, m_scale);

        const unsigned img_w = m_imageList->GetWidth();
        const unsigned img_h = m_imageList->GetHeight();

        const float half_w = (img_w * 0.5f);
        const float half_h = ((img_h) * 0.5f);
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

    }

    m_infoBar->Render();

    m_pixelInfo->Render();

    glutSwapBuffers();
}

void CWindow::fnResize(int width, int height)
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
    //printf("%d x %d -> %.2f x %.2f\n", width, height, m_viewport_w, m_viewport_h);
}

void CWindow::fnMouse(int x, int y)
{
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

void CWindow::fnMouseWheel(int /*wheel*/, int direction, int /*x*/, int /*y*/)
{
    if(direction > 0)
    {
        updateScale(true);
    }
    else //if(direction < 0)
    {
        updateScale(false);
    }
}

void CWindow::fnMouseButtons(int button, int state, int x, int y)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        m_mouseLB = (state == GLUT_DOWN);
        if(m_pixelInfo->IsVisible())
        {
            const cVector<float> pos(x / m_scale, y / m_scale);
            const cVector<float> point = screenToImage(pos);
            m_selection->MouseButton(point.x, point.y, m_mouseLB);
            updatePixelInfo(pos);
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        m_mouseMB = (state == GLUT_DOWN);
        break;

    case GLUT_RIGHT_BUTTON:
        m_mouseRB = (state == GLUT_DOWN);
        break;
    }
}

void CWindow::fnKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
    //GLUT_ACTIVE_SHIFT
    //GLUT_ACTIVE_CTRL
    //GLUT_ACTIVE_ALT
    int mod = glutGetModifiers();

    switch(key)
    {
    case 27: // ESC
        //cRenderer::disable(true);
        exit(0);
        break;

    case 'i':
    case 'I':
        m_infoBar->Show(!m_infoBar->Visible());
        //calculateScale();
        centerWindow();
        break;
    case 'p':
    case 'P':
        m_pixelInfo->Show(!m_pixelInfo->IsVisible());
        showCursor(!m_pixelInfo->IsVisible());
        break;
    case 's':
    case 'S':
        m_fitImage = !m_fitImage;
        if(m_fitImage == false)
        {
            m_scale = 1.0f;
        }
        m_camera = cVector<float>();
        centerWindow();
        updateInfobar();
        break;
    case ' ':
        loadImage(1);
        break;
    case key_backspace:
        loadImage(-1);
        break;
    case key_delete:
        if(mod == GLUT_ACTIVE_CTRL)
        {
            m_filesList->RemoveFromDisk();
        }
        break;
    case 'b':
    case 'B':
        m_showBorder = !m_showBorder;
        break;
    case '+':
    case '=':
        updateScale(true);
        break;
    case '-':
        updateScale(false);
        break;
    case 'c':
    case 'C':
        m_checkerBoard->Enable(!m_checkerBoard->IsEnabled());
        break;
    case '0':
        m_scale = 1.0f;
        m_camera = cVector<float>();
        m_fitImage = false;
        centerWindow();
        updateInfobar();
        break;
    case 13:
        m_windowed = !m_windowed;
        if(m_windowed)
        {
            centerWindow();
        }
        else
        {
            m_testFullscreen = true;

            storeWindowPositionSize(true, true);

            glutFullScreen();
        }
        break;
    case 'h':
        keyLeft();
        break;
    case 'l':
        keyRight();
        break;
    case 'k':
        keyUp();
        break;
    case 'j':
        keyDown();
        break;
    case 'R':
        m_angle += 360 - 90;
        m_angle %= 360;
        calculateScale();
        break;
    case 'L':
        m_angle += 90;
        m_angle %= 360;
        calculateScale();
        break;
    //default:
         //std::cout << key << std::endl;
         //break;
    }
}

void CWindow::fnKeyboardSpecial(int key, int /*x*/, int /*y*/)
{
    //std::cout << key << std::endl;
    switch(key)
    {
    case GLUT_KEY_LEFT:
        keyLeft();
        break;
    case GLUT_KEY_RIGHT:
        keyRight();
        break;
    case GLUT_KEY_UP:
        keyUp();
        break;
    case GLUT_KEY_DOWN:
        keyDown();
        break;
    case GLUT_KEY_PAGE_UP:
        loadImage(0, m_imageList->GetSub() - 1);
        break;
    case GLUT_KEY_PAGE_DOWN:
        loadImage(0, m_imageList->GetSub() + 1);
        break;
    //default:
         //std::cout << key << std::endl;
         //break;
    }
}

void CWindow::keyUp()
{
    shiftCamera(cVector<float>(0, -10));
}

void CWindow::keyDown()
{
    shiftCamera(cVector<float>(0, 10));
}

void CWindow::keyLeft()
{
    shiftCamera(cVector<float>(-10, 0));
}

void CWindow::keyRight()
{
    shiftCamera(cVector<float>(10, 0));
}

void CWindow::shiftCamera(const cVector<float>& delta)
{
    m_camera += delta;

    const unsigned w = m_imageList->GetWidth();
    const unsigned h = m_imageList->GetHeight();

    cVector<float> half = (m_viewport / m_scale + cVector<float>(w, h)) * 0.5f;
    m_camera.x = std::max<float>(m_camera.x, -half.x);
    m_camera.x = std::min<float>(m_camera.x, half.x);
    m_camera.y = std::max<float>(m_camera.y, -half.y);
    m_camera.y = std::min<float>(m_camera.y, half.y);
}


void CWindow::calculateScale()
{
    if(m_fitImage == true)
    {
        float w = static_cast<float>(m_imageList->GetWidth());
        float h = static_cast<float>(m_imageList->GetHeight());
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
    }

    updateFiltering();
}

// TODO update m_camera_x / m_camera_y according current mouse position
void CWindow::updateScale(bool up)
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

    updateFiltering();
    updateInfobar();
}

void CWindow::updateFiltering()
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

void CWindow::storeWindowPositionSize(bool position, bool size)
{
    if(position)
    {
        const int x = glutGet(GLUT_WINDOW_X);
        const int y = glutGet(GLUT_WINDOW_Y);
        m_prev_pos = cVector<float>(x, y);
    }

    if(size)
    {
        const int w = glutGet(GLUT_WINDOW_WIDTH);
        const int h = glutGet(GLUT_WINDOW_HEIGHT);
        m_prev_size = cVector<float>(w, h);
    }
}

void CWindow::centerWindow()
{
    if(m_windowed)
    {
        int winw = m_prev_size.x;
        int winh = m_prev_size.y;
        int posx = m_prev_pos.x;
        int posy = m_prev_pos.y;

        if(m_center_window)
        {
            // calculate window size
            int w = m_imageList->GetWidth();
            int h = m_imageList->GetHeight();
            int scrw = glutGet(GLUT_SCREEN_WIDTH);
            int scrh = glutGet(GLUT_SCREEN_HEIGHT);
            int imgw = std::max<int>(w + (m_showBorder ? m_border->GetBorderWidth() * 2 : 0), DEF_WINDOW_W);
            int imgh = std::max<int>(h + (m_showBorder ? m_border->GetBorderWidth() * 2 : 0), DEF_WINDOW_H);
            winw = std::min<int>(imgw, scrw);
            winh = std::min<int>(imgh, scrh);

            // calculate window position
            posx = (scrw - winw) / 2;
            posy = (scrh - winh) / 2;
        }

        glutPositionWindow(posx, posy);
        glutReshapeWindow(winw, winh);
        //printf("screen: %d x %d, window %d x %d, pos: %d, %d\n", scrw, scrh, winw, winh, posx, posy);

        m_prev_pos = cVector<float>(posx, posy);
        m_prev_size = cVector<float>(winw, winh);

        calculateScale();
    }
}

bool CWindow::loadImage(int step, int subImage)
{
    bool ret = false;

    m_na->Enable(false);

    if(step != 0)
    {
        m_scale = 1;
        m_angle = 0;
        m_camera = cVector<float>(0, 0);
        m_filesList->ParseDir();
    }

    const char* path = m_filesList->GetName(step);
    m_progress->Start();

    if(true == m_imageList->LoadImage(path, subImage))
    {
        createTextures();
        ret = true;
    }
    else
    {
        m_na->Enable(true);
    }

    m_selection->SetImageDimension(m_imageList->GetWidth(), m_imageList->GetHeight());
    updateInfobar();

    centerWindow();

    updatePixelInfo(m_lastMouse);

    //m_imageList->FreeMemory();

    return ret;
}

void CWindow::updateInfobar()
{
    calculateScale();

    INFO_BAR s;
    const char* path = m_filesList->GetName(0);
    s.path           = path;
    s.width          = m_imageList->GetWidth();
    s.height         = m_imageList->GetHeight();
    s.bpp            = m_imageList->GetImageBpp();
    s.scale          = m_scale;
    s.sub_image      = m_imageList->GetSub();
    s.sub_count      = m_imageList->GetSubCount();
    s.file_size      = m_imageList->GetSize();
    s.mem_size       = m_imageList->GetSizeMem();
    s.index          = m_filesList->GetIndex();
    s.files_count    = m_filesList->GetCount();
    m_infoBar->Update(&s);
}

const cVector<float> CWindow::screenToImage(const cVector<float>& pos)
{
    const float w = m_imageList->GetWidth();
    const float h = m_imageList->GetHeight();

    return pos + m_camera - (m_viewport / m_scale - cVector<float>(w, h)) * 0.5f;
}

void CWindow::updatePixelInfo(const cVector<float>& pos)
{
    if(m_imageList->GetBitmap())
    {
        const int w = m_imageList->GetWidth();
        const int h = m_imageList->GetHeight();

        const cVector<float> point = screenToImage(pos);
        const int x = (int)point.x;
        const int y = (int)point.y;

        sPixelInfo pixelInfo;

        // TODO check pixel format (RGB or BGR)
        if(x >= 0 && y >= 0 && x <= w && y <= h)
        {
            const int bpp = m_imageList->GetBpp();
            const int pitch = m_imageList->GetPitch();
            const size_t idx = (size_t)(x * bpp / 8 + y * pitch);
            const unsigned char* color = m_imageList->GetBitmap() + idx;
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

void CWindow::createTextures()
{
    unsigned char* bitmap = m_imageList->GetBitmap();
    if(bitmap)
    {
        const int width   = m_imageList->GetWidth();
        const int height  = m_imageList->GetHeight();
        const int format  = m_imageList->GetBitmapFormat();
        const int pitch   = m_imageList->GetPitch();
        const int bytesPP = (m_imageList->GetBpp() / 8);

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

void CWindow::showCursor(bool show)
{
    if(m_cursorVisible != show)
    {
        m_cursorVisible = show;
        glutSetCursor(show == true ? GLUT_CURSOR_LEFT_ARROW : GLUT_CURSOR_NONE);
    }
}

void CWindow::deleteTextures()
{
    for(size_t i = 0, size = m_quads.size(); i < size; i++)
    {
        delete m_quads[i];
    }
    m_quads.clear();
}

void CWindow::doProgress(int percent)
{
    m_progress->Render(percent);
}

