/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "viewer.h"
#include "common/config.h"

#include <GLFW/glfw3.h>

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

namespace
{

    const char* SimpleViewerTitle = "Simple Viewer GL";
    const float SimpleViewerVersion = 2.8;

    cViewer* m_viewer = nullptr;

    void showVersion()
    {
        printf("%s v%.2f\n\n", SimpleViewerTitle, SimpleViewerVersion);
        printf("Copyright © 2008-2017 Andrey A. Ugolnik. All Rights Reserved.\n");
        printf("http://www.ugolnik.info\n");
        printf("andrey@ugolnik.info\n");
    }

    const char* getValue(bool enabled)
    {
        return enabled ? "enabled" : "disabled";
    }

    void showHelp(const char* name, const sConfig& config)
    {
        const char* p = strrchr(name, '/');

        printf("\nUsage:\n");
        printf("  %s [OPTION]... FILE\n", (p != nullptr ? p + 1 : name));
        printf("  -h, --help     show this help;\n");
        printf("  -v, --version  show viewer version;\n");
        printf("  -s             enable scale to window (default: %s);\n", getValue(config.fitImage));
        printf("  -cw            center window (default: %s);\n", getValue(config.centerWindow));
        printf("  -a             do not filter by file extension;\n");
        printf("  -c             disable chequerboard (default: %s);\n", getValue(!config.hideCheckboard));
        printf("  -i             disable on-screen info (default: %s);\n", getValue(!config.hideInfobar));
        printf("  -p             show pixel info (pixel color and coordinates, default: %s);\n", getValue(config.showPixelInfo));
        printf("  -b             show border around image (default: %s);\n", getValue(config.showImageBorder));
        printf("  -f             start in fullscreen mode;\n");
        printf("  -r             recursive directory scan (default: %s);\n", getValue(config.recursiveScan));
        printf("  -wz            enable wheel zoom (default: %s);\n", getValue(config.wheelZoom));
        printf("  -mipmap VALUE  min texture size for mipmap creation (default: %u px);\n", config.mipmapTextureSize);
        printf("  -C RRGGBB      background color in hex format (default: %.2X%.2X%.2X);\n"
               , (unsigned)(config.color.r * 255)
               , (unsigned)(config.color.g * 255)
               , (unsigned)(config.color.b * 255));

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

#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 1
    void callbackDrop(GLFWwindow* /*window*/, int count, const char** paths)
    {
        m_viewer->addPaths(paths, count);
    }
#endif

    void setup(GLFWwindow* window)
    {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        glfwSetWindowSizeCallback(window, callbackResize);
        //glfwSetFramebufferSizeCallback(window, callbackResize);
        glfwSetWindowPosCallback(window, callbackPosition);

        glfwSetWindowRefreshCallback(window, callbackRedraw);

        glfwSetKeyCallback(window, callbackKeyboard);

        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
        glfwSetMouseButtonCallback(window, callbackMouseButtons);
        glfwSetCursorPosCallback(window, callbackMouse);
        glfwSetScrollCallback(window, callbackMouseScroll);

#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 1
        glfwSetDropCallback(window, callbackDrop);
#endif
    }

}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    sConfig config;

    for (int i = 1; i < argc; i++)
    {
        if (!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6))
        {
            showVersion();
            showHelp(argv[0], config);
            return 0;
        }
        else if (!strncmp(argv[i], "-v", 2) || !strncmp(argv[i], "--version", 9))
        {
            showVersion();
            return 0;
        }
    }

    cConfig reader;
    reader.read(config);

    const char* path = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "--debug", 7) == 0)
        {
            config.debug = true;
        }
        if (strncmp(argv[i], "-i", 2) == 0)
        {
            config.hideInfobar = true;
        }
        else if (strncmp(argv[i], "-p", 2) == 0)
        {
            config.showPixelInfo = true;
        }
        else if (strncmp(argv[i], "-cw", 3) == 0)
        {
            config.centerWindow = true;
        }
        else if (strncmp(argv[i], "-c", 2) == 0)
        {
            config.hideCheckboard = true;
        }
        else if (strncmp(argv[i], "-s", 2) == 0)
        {
            config.fitImage = true;
        }
        else if (strncmp(argv[i], "-b", 2) == 0)
        {
            config.showImageBorder = true;
        }
        else if (strncmp(argv[i], "-r", 2) == 0)
        {
            config.recursiveScan = true;
        }
        else if (strncmp(argv[i], "-a", 2) == 0)
        {
            config.skipFilter = true;
        }
        else if (strncmp(argv[i], "-wz", 3) == 0)
        {
            config.wheelZoom = true;
        }
        else if (strncmp(argv[i], "-C", 2) == 0)
        {
            int r, g, b;
            if (3 == sscanf(argv[i + 1], "%2x%2x%2x", &r, &g, &b))
            {
                config.color = { r / 255.0f, g / 255.0f, b / 255.0f };
                i++;
            }
        }
        else if (strncmp(argv[i], "-mipmap", 7) == 0)
        {
            if (i + 1 < argc)
            {
                config.mipmapTextureSize = (unsigned)::atoi(argv[++i]);
            }
        }
        else
        {
            path = argv[i];
        }
    }

    cViewer viewer(&config);
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

            bool updateSizePos = false;

            while (!glfwWindowShouldClose(window))
            {
                if (viewer.isWindowModeRequested())
                {
                    GLFWwindow* newWindow = nullptr;

                    const bool windowed = viewer.isWindowed();
                    if (windowed == false)
                    {
                        updateSizePos = true;
                        const auto& size = viewer.getWindowSize();
                        newWindow = glfwCreateWindow(size.x, size.y, SimpleViewerTitle, nullptr, window);
                    }
                    else
                    {
                        viewer.setWindowed(false);

                        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                        newWindow = glfwCreateWindow(mode->width, mode->height, SimpleViewerTitle, monitor, window);
                    }

                    setup(newWindow);
                    viewer.setWindow(newWindow);

                    glfwDestroyWindow(window);
                    window = newWindow;
                }
                else if (updateSizePos)
                {
                    updateSizePos = false;
                    viewer.setWindowed(true);

                    const auto& size = viewer.getWindowSize();
                    const auto& pos = viewer.getWindowPosition();
                    glfwSetWindowSize(window, size.x, size.y);
                    glfwSetWindowPos(window, pos.x, pos.y);
                    // ::printf("- updated to : %d x %d , size %d x %d\n", pos.x, pos.y, size.x, size.y);
                }

                const float start = glfwGetTime();
                static float end = start;

                viewer.render();
                viewer.update();

                glfwPollEvents();

                if (viewer.isUploading() == false)
                {
                    const float delta = end - start;

                    const float disiredFps = 1.0f / 60.0f;
                    const float time_rest = disiredFps - delta;
                    if (time_rest > 0.0f)
                    {
                        usleep(time_rest * 1000000);
                    }
                }

                end = glfwGetTime();
            }
        }

        glfwTerminate();
    }

    return 0;
}
