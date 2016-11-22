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
static const float SimpleViewerVersion = 2.71f;

static cViewer* m_viewer = nullptr;

void showHelp(const char* name);

void callbackResize(GLFWwindow* /*window*/, int width, int height)
{
    m_viewer->fnResize(width, height);
}

void callbackPosition(GLFWwindow* /*window*/, int x, int y)
{
    m_viewer->fnPosition(x, y);
}

void callbackRedraw(GLFWwindow* /*window*/)
{
    m_viewer->render();
}

void callbackKeyboard(GLFWwindow* /*window*/, int key, int scancode, int action, int mods)
{
    m_viewer->fnKeyboard(key, scancode, action, mods);
}

void callbackMouseButtons(GLFWwindow* /*window*/, int button, int action, int mods)
{
    m_viewer->fnMouseButtons(button, action, mods);
}

void callbackMouse(GLFWwindow* /*window*/, double x, double y)
{
    m_viewer->fnMouse(x, y);
}

void callbackMouseScroll(GLFWwindow* /*window*/, double x, double y)
{
    m_viewer->fnMouseScroll(x, y);
}

void callbackDrop(GLFWwindow* /*window*/, int count, const char** paths)
{
    m_viewer->addPaths(paths, count);
}

void setup(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1);

    glfwSetWindowSizeCallback(window, callbackResize);
    //glfwSetFramebufferSizeCallback(window, callbackResize);
    glfwSetWindowPosCallback(window, callbackPosition);

    glfwSetWindowRefreshCallback(window, callbackRedraw);

    glfwSetKeyCallback(window, callbackKeyboard);

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
    glfwSetMouseButtonCallback(window, callbackMouseButtons);
    glfwSetCursorPosCallback(window, callbackMouse);
    glfwSetScrollCallback(window, callbackMouseScroll);

    glfwSetDropCallback(window, callbackDrop);
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    printf("%s v%.2f\n\n", SimpleViewerTitle, SimpleViewerVersion);
    printf("Copyright © 2008-2016 Andrey A. Ugolnik. All Rights Reserved.\n");
    printf("http://www.ugolnik.info\n");
    printf("andrey@ugolnik.info\n\n");

    for (int i = 1; i < argc; i++)
    {
        if (!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6))
        {
            showHelp(argv[0]);
            return 0;
        }
    }

    cConfig config;
    config.read();
    sConfig& c = config.getConfig();

    const char* path = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-i", 2) == 0)
        {
            c.hideInfobar = true;
        }
        else if (strncmp(argv[i], "-p", 2) == 0)
        {
            c.showPixelInfo = true;
        }
        else if (strncmp(argv[i], "-cw", 3) == 0)
        {
            c.centerWindow = true;
        }
        else if (strncmp(argv[i], "-c", 2) == 0)
        {
            c.hideCheckboard = true;
        }
        else if (strncmp(argv[i], "-s", 2) == 0)
        {
            c.fitImage = true;
        }
        //else if(strncmp(argv[i], "-f", 2) == 0)
        //c.fullScreen = true;
        else if (strncmp(argv[i], "-b", 2) == 0)
        {
            c.showImageBorder = true;
        }
        else if (strncmp(argv[i], "-r", 2) == 0)
        {
            c.recursiveScan = true;
        }
        else if (strncmp(argv[i], "-a", 2) == 0)
        {
            c.skipFilter = true;
        }
        else if (strncmp(argv[i], "-wz", 3) == 0)
        {
            c.wheelZoom = true;
        }
        else if (strncmp(argv[i], "-C", 2) == 0)
        {
            int r, g, b;
            if (3 == sscanf(argv[i + 1], "%2x%2x%2x", &r, &g, &b))
            {
                c.color = { r / 255.0f, g / 255.0f, b / 255.0f };
                i++;
            }
        }
        else
        {
            path = argv[i];
        }
    }

    cViewer viewer(&c);
    m_viewer = &viewer;

    viewer.setInitialImagePath(path);

    if (glfwInit())
    {
        GLFWwindow* window = glfwCreateWindow(640, 480, SimpleViewerTitle, nullptr, nullptr);
        if (window != nullptr)
        {
            setup(window);
            viewer.setWindow(window);
            viewer.loadImage(0);

            while (!glfwWindowShouldClose(window))
            {
                if (viewer.isWindowModeRequested())
                {
                    GLFWwindow* newWindow = nullptr;

                    const bool windowed = viewer.isWindowed();
                    if (windowed)
                    {
                        const auto& size = viewer.getWindowSize();
                        newWindow = glfwCreateWindow(size.x, size.y, SimpleViewerTitle, nullptr, window);

                        const auto& pos = viewer.getWindowPosition();
                        glfwSetWindowPos(newWindow, pos.x, pos.y);
                    }
                    else
                    {
                        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                        newWindow = glfwCreateWindow(mode->width, mode->height, SimpleViewerTitle, monitor, window);
                    }

                    setup(newWindow);
                    viewer.setWindow(newWindow);

                    glfwDestroyWindow(window);
                    window = newWindow;
                }

                viewer.render();
                glfwPollEvents();
            }
        }

        glfwTerminate();
    }

    return 0;
}

void showHelp(const char* name)
{
    const char* p = strrchr(name, '/');

    printf("\nUsage:\n");
    printf("  %s [OPTION]... FILE\n", (p != nullptr ? p + 1 : name));
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
    printf("  -wz           enable wheel zoom;\n");
    printf("  -C RRGGBB     background color in hex format;\n");

    printf("\nAvailable keys:\n");
    printf("  <esc>         exit;\n");
    printf("  <space>       next image;\n");
    printf("  <backspace>   previous image;\n");
    printf("  <+> / <->     scale image;\n");
    printf("  <1>...<0>     set scale from 100%% to 1000%%;\n");
    printf("  <pgdn>        next image in multi-page image;\n");
    printf("  <pgup>        previous image in multi-page image;\n");
    printf("  <enter>       switch fullscreen / windowed mode;\n");
    printf("  <ctrl>+<del>  delete image from disk;\n");
    printf("  <s>           fit image to window (quick algorithm);\n");
    printf("  <r>           rotate clockwise;\n");
    printf("  <shift>+<r>   rotate counterclockwise;\n");
    printf("  <c>           hide / show chequerboard;\n");
    printf("  <i>           hide / show on screen info;\n");
    printf("  <p>           hide / show pixel info;\n");
    printf("  <b>           hide / show border around image;\n");
    printf("\n");
}
