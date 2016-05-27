/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "viewer.h"
#include "config.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <clocale>
#include <cstring>
#include <cstdio>

static const char* SimpleViewerTitle = "Simple Viewer GL";
static cViewer* m_viewer = nullptr;

void showHelp(const char* name);

void callbackResize(GLFWwindow* window, int width, int height)
{
    (void)window;
    (void)width;
    (void)height;
    m_viewer->fnResize();
}

void callbackMouse(GLFWwindow* window, double x, double y)
{
    (void)window;
    m_viewer->fnMouse(x, y);
}

void callbackMouseButtons(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    m_viewer->fnMouseButtons(button, action, mods);
}

void callbackKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    m_viewer->fnKeyboard(key, scancode, action, mods);
}

void setup(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowSizeCallback(window, callbackResize);
    glfwSetFramebufferSizeCallback(window, callbackResize);
    //glfwSetWindowPosCallback(window, callbackPosition);
    glfwSetCursorPosCallback(window, callbackMouse);
    glfwSetKeyCallback(window, callbackKeyboard);
    glfwSetMouseButtonCallback(window, callbackMouseButtons);
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

    cViewer viewer;
    m_viewer = &viewer;

    try
    {
        CConfig config(&viewer);
        if(config.Open() == true)
        {
            config.Read();
        }
    }
    catch(...)
    {
        printf("Error loading config.\n\n");
    }

    const char* path = nullptr;

    for(int i = 1; i < argc; i++)
    {
        if(strncmp(argv[i], "-i", 2) == 0)
            viewer.SetProp(cViewer::Property::Infobar);
        else if(strncmp(argv[i], "-p", 2) == 0)
            viewer.SetProp(cViewer::Property::PixelInfo);
        else if(strncmp(argv[i], "-cw", 3) == 0)
            viewer.SetProp(cViewer::Property::CenterWindow);
        else if(strncmp(argv[i], "-c", 2) == 0)
            viewer.SetProp(cViewer::Property::Checkers);
        else if(strncmp(argv[i], "-s", 2) == 0)
            viewer.SetProp(cViewer::Property::FitImage);
        else if(strncmp(argv[i], "-f", 2) == 0)
            viewer.SetProp(cViewer::Property::Fullscreen);
        else if(strncmp(argv[i], "-b", 2) == 0)
            viewer.SetProp(cViewer::Property::Border);
        else if(strncmp(argv[i], "-r", 2) == 0)
            viewer.SetProp(cViewer::Property::Recursive);
        else if(strncmp(argv[i], "-a", 2) == 0)
            viewer.SetProp(cViewer::Property::AllValid);
        else if(strncmp(argv[i], "-C", 2) == 0)
        {
            unsigned int r, g, b;
            if(3 == sscanf(argv[i + 1], "%2x%2x%2x", &r, &g, &b))
            {
                viewer.SetProp(r, g, b);
                i++;
            }
        }
        else
        {
            path = argv[i];
        }
    }

    if(viewer.setInitialImagePath(path))
    {
        if(glfwInit())
        {
            GLFWwindow* window = glfwCreateWindow(640, 480, SimpleViewerTitle, nullptr, nullptr);
            if(window != nullptr)
            {
                setup(window);
                viewer.setWindow(window);

                while(!glfwWindowShouldClose(window) && !viewer.isQuitRequested())
                {
                    if(viewer.isWindowModeRequested())
                    {
                        GLFWwindow* newWindow = nullptr;

                        const bool windowed = viewer.isWindowed();
                        if(windowed)
                        {
                            const cVector<float>& size = viewer.getWindowSize();
                            newWindow = glfwCreateWindow(size.x, size.y, SimpleViewerTitle, nullptr, window);

                            const cVector<float>& pos = viewer.getWindowPosition();
                            glfwSetWindowPos(newWindow, pos.x, pos.y);
                        }
                        else
                        {
                            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                            newWindow = glfwCreateWindow(mode->width, mode->height, SimpleViewerTitle, monitor, window);
                        }

                        glfwDestroyWindow(window);
                        window = newWindow;

                        setup(newWindow);
                        viewer.setWindow(newWindow);
                    }

                    viewer.render();
                    glfwPollEvents();
                }
            }

            glfwTerminate();
        }
    }

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

