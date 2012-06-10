/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "window.h"
#include "main.h"
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

//void closeWindow() {
//	printf("done\n");
//}

// static members of CWindow class
CWindow* m_window = 0;

CWindow::CWindow()
    : m_initialImageLoading(true)
    , m_prevWinX(0)
    , m_prevWinY(0)
    , m_prevWinW(DEF_WINDOW_W)
    , m_prevWinH(DEF_WINDOW_H)
    , m_scale(1)
    , m_windowed(true)
    , m_center_window(false)
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
    m_window = this;
    m_lastMouse = cVector(-1, -1);

    m_imageList.reset(new CImageLoader(callbackProgressLoading));
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
    m_window = 0;
}

bool CWindow::Init(int argc, char* argv[], const char* path)
{
    m_filesList.reset(new CFilesList(path, m_recursiveDir));
    if(m_filesList->GetName() != 0)
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

        glutCreateWindow(DEF_TITLE);

        //int version = glutGet(GLUT_VERSION);
        //std::cout << "GLUT v" << version << std::endl;

        cRenderer::init();

        m_checkerBoard->Init();
        m_na->Init();
        m_infoBar->Init();
        m_pixelInfo->Init();
        updatePixelInfo(cVector(DEF_WINDOW_W, DEF_WINDOW_H));
        m_progress->Init();
        m_selection->Init();

        glutReshapeFunc(callbackResize);
        glutDisplayFunc(callbackRender);
        glutTimerFunc(100, callbackTimerUpdate, 100);
        glutTimerFunc(2000, callbackTimerCursor, 1);
        glutKeyboardFunc(callbackKeyboard);
        glutMouseFunc(callbackMouseButtons);
        glutSpecialFunc(callbackKeyboardSpecial);
        //glutEntryFunc();
        glutMotionFunc(callbackMouse);
        glutPassiveMotionFunc(callbackMouse);
        //glutMouseWheelFunc(callbackMouseWheel);
        //glutWMCloseFunc(closeWindow);

        m_initialImageLoading = true;

        std::cout << std::endl;

        glutMainLoop();

        return true;
    }
    return false;
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
        int a_width = glutGet(GLUT_WINDOW_WIDTH);
        int a_height = glutGet(GLUT_WINDOW_HEIGHT);
        int scrw = glutGet(GLUT_SCREEN_WIDTH);
        int scrh = glutGet(GLUT_SCREEN_HEIGHT);
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
        float img_w = m_imageList->GetWidth();
        float img_h = m_imageList->GetHeight();

        cVector half = (m_viewport / m_scale + cVector(img_w, img_h) * m_scale) * 0.5f;
        half -= 20.0f;
        m_camera.x = std::max<int>(m_camera.x, -half.x);
        m_camera.x = std::min<int>(m_camera.x, half.x);
        m_camera.y = std::max<int>(m_camera.y, -half.y);
        m_camera.y = std::min<int>(m_camera.y, half.y);

        cVector pos(m_camera.x, m_camera.y + m_infoBar->GetHeight() / 2.0f);
        cRenderer::setGlobals(&pos, m_angle, m_scale);

        QuadsIc it = m_quads.begin(), itEnd = m_quads.end();
        for( ; it != itEnd; ++it)
        {
            CQuadImage* quad = *it;

            float x = quad->GetCol() * quad->GetTexWidth() - img_w * 0.5f;
            float y = quad->GetRow() * quad->GetTexHeight() - img_h * 0.5f;

            quad->Render(x, y);
        }

        if(m_showBorder == true)
        {
            m_border->Render(-img_w * 0.5f, -img_h * 0.5f, img_w, img_h, m_scale);
        }
        if(m_pixelInfo->IsVisible())
        {
            m_selection->Render(m_camera, m_scale);
            //m_selection->Render(0, 0);
        }
        cRenderer::setGlobals();

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
    //m_viewport_w = glutGet(GLUT_WINDOW_WIDTH);
    //m_viewport_h = glutGet(GLUT_WINDOW_HEIGHT) - m_infoBar->GetHeight();
    m_viewport = cVector(static_cast<float>(width), static_cast<float>(height - m_infoBar->GetHeight()));

    updateInfobar();

    //updateViewportSize();

    glViewport(0, 0, width, height);

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(0, m_viewport_w, m_viewport_h + m_infoBar->GetHeight(), 0, -1, 1);
    cRenderer::setGlobals();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_pixelInfo->SetWindowSize(m_viewport);
    //printf("%d x %d -> %.2f x %.2f\n", width, height, m_viewport_w, m_viewport_h);
}

void CWindow::fnMouse(int x, int y)
{
    bool forceUpdate = false;

    cVector diff(m_lastMouse - cVector(x, y));
    m_lastMouse = cVector(x, y);
    //if(!m_fitImage && (m_mouseMB || m_mouseRB))
    if(m_mouseMB || m_mouseRB)
    {
        if(diff != cVector())
        {
            forceUpdate = true;
            updatePosition(diff);
        }
    }

    if(m_pixelInfo->IsVisible())
    {
        forceUpdate = true;
        //if(m_scale == 1)
        {
            m_selection->MouseMove(x - m_camera.x, y - m_camera.y);
            int cursor = m_selection->GetCursor();
            m_pixelInfo->SetCursor(cursor);
        }

        updatePixelInfo(cVector(x, y));
    }
    else
    {
        showCursor(true);
    }

    if(forceUpdate == true)
    {
        glutPostRedisplay();
    }
}

void CWindow::fnMouseWheel(int /*wheel*/, int direction, int /*x*/, int /*y*/)
{
    if(direction > 0)
    {
        updateScale(true);
    }
    else
    {//if(direction < 0) {
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
            m_selection->MouseButton(x - m_camera.x, y - m_camera.y, m_mouseLB);
            //glutPostRedisplay();
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
            m_scale = 1;
        }
        centerWindow();
        updateInfobar();
        break;
    case ' ':
        loadImage(1);
        break;
        // backspace
#if defined(__APPLE__)
    case 127:
#else
    case 8:
#endif
        loadImage(-1);
        break;
        // Delete
#if defined(__APPLE__)
    case 8:
#else
    case 127:
#endif
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
        m_scale = 1;
        m_fitImage = false;
        centerWindow();
        updateInfobar();
        break;
    case 13:
        m_windowed = !m_windowed;
        if(m_windowed == false)
        {
            m_testFullscreen = true;

            glutFullScreen();
        }
        else
        {
            centerWindow();
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
        break;
    case 'L':
        m_angle += 90;
        m_angle %= 360;
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
        //glutPostRedisplay();
        break;
    case GLUT_KEY_PAGE_DOWN:
        loadImage(0, m_imageList->GetSub() + 1);
        //glutPostRedisplay();
        break;
    //default:
         //std::cout << key << std::endl;
         //break;
    }
}

void CWindow::keyUp()
{
    updatePosition(cVector(0, -10));
}

void CWindow::keyDown()
{
    updatePosition(cVector(0, 10));
}

void CWindow::keyLeft()
{
    updatePosition(cVector(-10, 0));
}

void CWindow::keyRight()
{
    updatePosition(cVector(10, 0));
}

void CWindow::updatePosition(const cVector& _delta)
{
    float scale = 1.0f / m_scale;
    m_camera += (_delta * scale);
}


void CWindow::calculateScale()
{
    if(m_fitImage == true)
    {
        float w = static_cast<float>(m_imageList->GetWidth());
        float h = static_cast<float>(m_imageList->GetHeight());

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
            m_scale = 1;
        }
    }

    //float w = m_imageList->GetWidth();
    //float h = m_imageList->GetHeight();
    //if(m_scale >= 1)
    //{
        //if(m_fitImage == true)
        //{
            //m_camera = cVector(ceilf((m_viewport_w * m_scale - w) / 2), ceilf((m_viewport_h * m_scale - h) / 2));
        //}
        //else
        //{
            //m_camera = cVector(ceilf((m_viewport_w / m_scale - w) / 2), ceilf((m_viewport_h / m_scale - h) / 2));
        //}
    //}
    //else
    //{
        //m_camera = cVector(ceilf((m_viewport_w / m_scale - w) / 2), ceilf((m_viewport_h / m_scale - h) / 2));
    //}

    updateFiltering();
}

// TODO update m_camera_x / m_camera_y according current mouse position
void CWindow::updateScale(bool up)
{
    m_fitImage = false;

    int scale = ceilf(m_scale * 10) * 10;

    if(up == true)
    {
        scale += 20;
    }
    else
    {
        if(scale > 20)
        {
            scale -= 20;
        }
    }
    m_scale = scale / 100.0f;

    updateFiltering();
    updateInfobar();
}

void CWindow::updateFiltering()
{
    int scale = static_cast<int>(m_scale * 100);
    if(scale % 100 == 0 && m_scale >= 1.0f)
    {
        for(QuadsIc it = m_quads.begin(), itEnd = m_quads.end(); it != itEnd; ++it)
        {
            (*it)->useFilter(false);
        }
    }
    else
    {
        for(QuadsIc it = m_quads.begin(), itEnd = m_quads.end(); it != itEnd; ++it)
        {
            (*it)->useFilter(true);
        }
    }
}

void CWindow::centerWindow()
{
    if(m_windowed && m_center_window)
    {
        calculateScale();
        int w = m_imageList->GetWidth();// * m_scale;
        int h = m_imageList->GetHeight();// * m_scale;
        int scrw = glutGet(GLUT_SCREEN_WIDTH);
        int scrh = glutGet(GLUT_SCREEN_HEIGHT);
        int imgw = std::max<int>(w + (m_showBorder ? m_border->GetBorderWidth() * 2 : 0), DEF_WINDOW_W);
        int imgh = std::max<int>(h + (m_showBorder ? m_border->GetBorderWidth() * 2 : 0) + m_infoBar->GetHeight(), DEF_WINDOW_H);
        int winw = std::min<int>(imgw, scrw);
        int winh = std::min<int>(imgh, scrh);
        glutReshapeWindow(winw, winh);

        int posx = (scrw - winw) / 2;
        int posy = (scrh - winh) / 2;
        glutPositionWindow(posx, posy);

        //printf("screen: %d x %d, window %d x %d, pos: %d, %d\n", scrw, scrh, winw, winh, posx, posy);
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
        m_camera = cVector(0, 0);
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
    const char* path    = m_filesList->GetName(0);
    s.path              = path;
    s.width             = m_imageList->GetWidth();
    s.height            = m_imageList->GetHeight();
    s.bpp               = m_imageList->GetImageBpp();
    s.scale             = m_scale;
    s.sub_image         = m_imageList->GetSub();
    s.sub_count         = m_imageList->GetSubCount();
    s.file_size         = m_imageList->GetSize();
    s.mem_size          = m_imageList->GetSizeMem();
    s.index             = m_filesList->GetIndex();
    s.files_count       = m_filesList->GetCount();
    m_infoBar->Update(&s);
}

void CWindow::updatePixelInfo(const cVector& _pos)
{
    if(m_imageList->GetBitmap())
    {
        cVector cursor(_pos - m_viewport / 2);
        PixelInfo pixelInfo;
        pixelInfo.cursor = cursor;
        pixelInfo.img = //(_pos / m_scale - m_camera);
                cVector(m_imageList->GetWidth() / 2.0f, m_imageList->GetHeight() / 2.0f) - cursor;//* m_scale;

        // TODO check pixel format (RGB or BGR)
        if(pixelInfo.img.x >= 0
                && pixelInfo.img.y >= 0
                && pixelInfo.img.x < m_imageList->GetHeight()
                && pixelInfo.img.y < m_imageList->GetWidth())
        {
            unsigned char* color = m_imageList->GetBitmap();
            color += static_cast<size_t>(pixelInfo.img.x * m_imageList->GetBpp() / 8 + pixelInfo.img.y * m_imageList->GetPitch());
            pixelInfo.r = color[0];
            pixelInfo.g = color[1];
            pixelInfo.b = color[2];
            pixelInfo.a = m_imageList->GetBpp() == 32 ? color[3] : 255;
        }

        //pixelInfo.x         = x - m_camera_x;
        //pixelInfo.y         = y - m_camera_y;
        //pixelInfo.bitmap    = m_imageList->GetBitmap();
        pixelInfo.w         = m_imageList->GetWidth();
        pixelInfo.h         = m_imageList->GetHeight();
        //pixelInfo.pitch     = m_imageList->GetPitch();
        //pixelInfo.bpp       = m_imageList->GetBpp();
        //pixelInfo.format    = m_imageList->GetBitmapFormat();
        //pixelInfo.rc        = m_selection->GetRect();
        //pixelInfo.scale     = m_scale;
        m_pixelInfo->Update(&pixelInfo);
    }
}

void CWindow::createTextures()
{
    unsigned char* bitmap = m_imageList->GetBitmap();
    if(bitmap)
    {
        int width   = m_imageList->GetWidth();
        int height  = m_imageList->GetHeight();
        int format  = m_imageList->GetBitmapFormat();
        int pitch   = m_imageList->GetPitch();
        int bytesPP = (m_imageList->GetBpp() / 8);

        std::cout << " " << width << " x " << height << ", ";

        int texW, texH;
        cRenderer::calculateTextureSize(&texW, &texH, width, height);
        if(texW > 0 && texH > 0)
        {
            // texture pitch should be multiple by 4
            int texPitch = static_cast<int>(ceilf(texW * bytesPP / 4.0f) * 4);

            int cols = static_cast<int>(ceilf(static_cast<float>(width) / texW));
            int rows = static_cast<int>(ceilf(static_cast<float>(height) / texH));
            size_t quadsCount = cols * rows;
            std::cout << "textures: " << quadsCount << " (" << cols << " x " << rows << ") required" << std::endl;

            deleteTextures();

            unsigned char* buffer = new unsigned char[texPitch * texH];

            int idx = 0;
            int height2 = height;
            for(int row = 0; row < rows; row++)
            {
                int width2 = width;
                int h = (height2 > texH ? texH : height2);
                for(int col = 0; col < cols; col++)
                {
                    int w = (width2 > texW ? texW : width2);
                    width2 -= w;

                    int dx = col * texPitch;
                    int dy = row * texH;
                    int count = w * bytesPP;
                    for(int line = 0; line < h; line++)
                    {
                        int src	= dx + (dy + line) * pitch;
                        int dst	= line * texPitch;
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
        glutSetCursor(show == true ? GLUT_CURSOR_RIGHT_ARROW : GLUT_CURSOR_NONE);

        if(m_cursorVisible == true)
        {
            glutTimerFunc(2000, callbackTimerCursor, 1);
        }
    }
}

void CWindow::deleteTextures()
{
    for(QuadsIc it = m_quads.begin(), itEnd = m_quads.end(); it != itEnd; ++it)
    {
        delete (*it);
    }
    m_quads.clear();
}

//void CWindow::updateViewportSize()
//{
    //m_viewport_w = glutGet(GLUT_WINDOW_WIDTH);
    //m_viewport_h = glutGet(GLUT_WINDOW_HEIGHT) - m_infoBar->GetHeight();
//}


void CWindow::callbackResize(int width, int height)
{
    m_window->fnResize(width, height);
}

void CWindow::callbackRender()
{
    m_window->fnRender();
}

void CWindow::callbackTimerUpdate(int value)
{
    glutPostRedisplay();
    glutTimerFunc(100, callbackTimerUpdate, value);
}

void CWindow::callbackTimerCursor(int /*value*/)
{
    m_window->showCursor(false);
}

void CWindow::callbackMouse(int x, int y)
{
    m_window->fnMouse(x, y);
}

void CWindow::callbackMouseButtons(int button, int state, int x, int y)
{
    m_window->fnMouseButtons(button, state, x, y);
}

void CWindow::callbackMouseWheel(int wheel, int direction, int x, int y)
{
    m_window->fnMouseWheel(wheel, direction, x, y);
}

void CWindow::callbackKeyboardSpecial(int key, int x, int y)
{
    m_window->fnKeyboardSpecial(key, x, y);
}

void CWindow::callbackKeyboard(unsigned char key, int x, int y)
{
    m_window->fnKeyboard(key, x, y);
}

void CWindow::fnProgressLoading(int percent)
{
    m_progress->Render(percent);
}

void CWindow::callbackProgressLoading(int percent)
{
    m_window->fnProgressLoading(percent);
}

