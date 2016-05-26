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

#include <GLFW/glfw3.h>

#include <iostream>
#include <clocale>
#include <cstring>
#include <cstdio>

static const char* SimpleViewerTitle = "Simple Viewer GL";
static CWindow* m_window = nullptr;

void showHelp(const char* name);

void callbackResize(GLFWwindow* window, int width, int height)
{
    (void)window;
    m_window->fnResize(width, height);
}

void callbackMouse(GLFWwindow* window, double x, double y)
{
    (void)window;
    m_window->fnMouse(x, y);
}

void callbackMouseButtons(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    m_window->fnMouseButtons(button, action, mods);
}

void callbackKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    m_window->fnKeyboard(key, scancode, action, mods);
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    printf("%s v2.6\n\n", SimpleViewerTitle);
    printf("Copyright © 2008-2016 Andrey A. Ugolnik. All Rights Reserved.\n");
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
        if(glfwInit())
        {
            GLFWwindow* window = glfwCreateWindow(640, 480, SimpleViewerTitle, nullptr, nullptr);
            if(window != nullptr)
            {
                glfwMakeContextCurrent(window);

                glfwSetWindowSizeCallback(window, callbackResize);
                //glfwSetWindowPosCallback(window, callbackResize);
                glfwSetCursorPosCallback(window, callbackMouse);
                glfwSetKeyCallback(window, callbackKeyboard);
                glfwSetMouseButtonCallback(window, callbackMouseButtons);

                m_window->run(window);

                while(!glfwWindowShouldClose(window))
                {
                    m_window->fnRender();

                    glfwSwapBuffers(window);

                    glfwPollEvents();
                }
            }
            glfwTerminate();
        }

        //glutInit(&argc, argv);
        //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

        //glutCreateWindow(SimpleViewerTitle);

        //glutReshapeFunc(callbackResize);
        //glutDisplayFunc(callbackRender);
        //glutTimerFunc(1000/60, callbackTimerUpdate, 1000/60);
        //glutTimerFunc(2000, callbackTimerCursor, 2000);
        //glutKeyboardFunc(callbackKeyboard);
        //glutMouseFunc(callbackMouseButtons);
        //glutSpecialFunc(callbackKeyboardSpecial);
        ////glutEntryFunc();
        //glutMotionFunc(callbackMouse);
        //glutPassiveMotionFunc(callbackMouse);
        ////glutMouseWheelFunc(callbackMouseWheel);
        ////glutWMCloseFunc(closeWindow);

        //m_window->run();

        //glutMainLoop();
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

