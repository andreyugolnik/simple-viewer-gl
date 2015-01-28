/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "window.h"
#include "config.h"

#if defined(__linux__)
#   include <GL/glut.h>
#else
#   include <glut.h>
#endif

#include <iostream>
#include <locale.h>
#include <string.h>
#include <stdio.h>

static const char* DEF_TITLE = "Simple Viewer GL";
static CWindow* m_window = 0;

void showHelp(const char* name);

void callbackResize(int width, int height)
{
    m_window->fnResize(width, height);
}

void callbackRender()
{
    m_window->fnRender();
}

void callbackTimerUpdate(int value)
{
    glutPostRedisplay();
    glutTimerFunc(100, callbackTimerUpdate, value);

    // workaround: store window position by timer, because glut has not related callback
    m_window->storeWindowPositionSize(true, false);
}

void callbackTimerCursor(int /*value*/)
{
    m_window->showCursor(false);

    glutTimerFunc(2000, callbackTimerCursor, 1);
}

void callbackMouse(int x, int y)
{
    m_window->fnMouse(x, y);
}

void callbackMouseButtons(int button, int state, int x, int y)
{
    m_window->fnMouseButtons(button, state, x, y);
}

void callbackMouseWheel(int wheel, int direction, int x, int y)
{
    m_window->fnMouseWheel(wheel, direction, x, y);
}

void callbackKeyboardSpecial(int key, int x, int y)
{
    m_window->fnKeyboardSpecial(key, x, y);
}

void callbackKeyboard(unsigned char key, int x, int y)
{
    m_window->fnKeyboard(key, x, y);
}

// Program entry point
int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    printf("%s v2.3\n\n", DEF_TITLE);
    printf("Copyright © 2008-2014 Andrey A. Ugolnik. All Rights Reserved.\n");
    printf("http://www.ugolnik.info\n");
    printf("andrey@ugolnik.info\n\n");

    if(argc < 2)
    {
        showHelp(argv[0]);
        return 0;
    }

    for(int i = 1; i < argc; i++)
    {
        if(!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6))
        {
            showHelp(argv[0]);
            return 0;
        }
    }

    m_window = new CWindow();

    try
    {
        CConfig config(m_window);
        if(config.Open() == true)
        {
            config.Read();
        }
    }
    catch(...)
    {
        printf("Error loading config.\n\n");
    }

    const char* path = 0;

    for(int i = 1; i < argc; i++)
    {
        if(strncmp(argv[i], "-i", 2) == 0)
            m_window->SetProp(PROP_INFOBAR);
        else if(strncmp(argv[i], "-p", 2) == 0)
            m_window->SetProp(PROP_PIXELINFO);
        else if(strncmp(argv[i], "-cw", 3) == 0)
            m_window->SetProp(PROP_CENTER_WINDOW);
        else if(strncmp(argv[i], "-c", 2) == 0)
            m_window->SetProp(PROP_CHECKERS);
        else if(strncmp(argv[i], "-s", 2) == 0)
            m_window->SetProp(PROP_FITIMAGE);
        else if(strncmp(argv[i], "-f", 2) == 0)
            m_window->SetProp(PROP_FULLSCREEN);
        else if(strncmp(argv[i], "-b", 2) == 0)
            m_window->SetProp(PROP_BORDER);
        else if(strncmp(argv[i], "-r", 2) == 0)
            m_window->SetProp(PROP_RECURSIVE);
        else if(strncmp(argv[i], "-a", 2) == 0)
            m_window->SetProp(PROP_ALL_VALID);
        else if(strncmp(argv[i], "-C", 2) == 0)
        {
            unsigned int r, g, b;
            if(3 == sscanf(argv[i + 1], "%2x%2x%2x", &r, &g, &b))
            {
                m_window->SetProp(r, g, b);
                i++;
            }
        }
        else
        {
            path = argv[i];
        }
    }

    if(m_window->setInitialImagePath(path))
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

        glutCreateWindow(DEF_TITLE);

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

        m_window->run();

        glutMainLoop();
    }

    delete m_window;

    return 0;
}

void showHelp(const char* name)
{
    const char* p = strrchr(name, '/');

    printf("\nUsage:\n");
    printf("  %s [OPTION]... FILE\n", (p ? p + 1 : name));
    printf("  -h, --help    show this help;\n");
    printf("  -s            enable scale to window;\n");
    printf("  -cw           center window;\n");
    printf("  -a            do not filter by file ext;\n");
    printf("  -c            disable chequerboard;\n");
    printf("  -i            disable on screen info;\n");
    printf("  -p            show pixel info (pixel color and coordinates);\n");
    printf("  -b            show border around image;\n");
    printf("  -f            start in fullscreen mode;\n");
    printf("  -r            recursive directory scan;\n");
    printf("  -C RRGGBB     background color in hex format;\n");

    printf("\nAvailable keys:\n");
    printf("  <esc>         exit;\n");
    printf("  <space>       next image;\n");
    printf("  <backspace>   previous image;\n");
    printf("  <+> / <->     scale image;\n");
    printf("  <0>           set scale to 100%%;\n");
    printf("  <pgdn>        next image in multi-page image;\n");
    printf("  <pgup>        previous image in multi-page image;\n");
    printf("  <enter>       switch fullscreen / windowed mode;\n");
    //printf("  <ctrl>+<b>    set as wallpapper;\n");
    printf("  <ctrl>+<del>  delete image from disk;\n");
    printf("  <s>           fit image to window (quick algorithm);\n");
    //printf("  <v>           flip vertical;\n");
    //printf("  <h>           flip horizontal;\n");
    printf("  <R>           rotate clockwice;\n");
    printf("  <L>           rotate counter clockwise;\n");
    printf("  <c>           hide / show chequerboard;\n");
    printf("  <i>           hide / show on screen info;\n");
    printf("  <p>           hide / show pixel info;\n");
    printf("  <b>           hide / show border around image;\n");
    printf("\n");
}

