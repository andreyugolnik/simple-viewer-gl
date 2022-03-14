/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "common/config.h"
#include "types/types.h"
#include "version.h"
#include "viewer.h"

#include <GLFW/glfw3.h>

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

namespace
{
    cViewer* m_viewer = nullptr;

    void showVersion()
    {
        printf("%s %d.%d.%d\n\n", SVGL_Title, SVGL_VerMajor, SVGL_VerMinor, SVGL_VerRelease);
        printf("Copyright © 2008-2022 Andrey A. Ugolnik. All Rights Reserved.\n");
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
        printf("  -c             user defined background color (default #%.2x%.2x%.2x);\n", (uint32_t)config.bgColor.r, (uint32_t)config.bgColor.g, (uint32_t)config.bgColor.b);
        printf("  -i             disable on-screen info (default: %s);\n", getValue(!config.hideInfobar));
        printf("  -p             show pixel info (pixel color and coordinates, default: %s);\n", getValue(config.showPixelInfo));
        printf("  -e             show exif info (default: %s);\n", getValue(config.showExif));
        printf("  -b             show border around image (default: %s);\n", getValue(config.showImageBorder));
        printf("  -g             show image grid (default: %s);\n", getValue(config.showImageGrid));
        printf("  -f             start in fullscreen mode (default: %s);\n", getValue(config.fullScreen));
        printf("  -r             recursive directory scan (default: %s);\n", getValue(config.recursiveScan));
        printf("  -wz            enable wheel zoom (default: %s);\n", getValue(config.wheelZoom));
        printf("  -mipmap VALUE  min texture size for mipmap creation (default: %u px);\n", config.mipmapTextureSize);
        printf("  -svg SIZE      min SVG size (default: %g px);\n", config.minSvgSize);
        printf("  -C RRGGBB      background color in hex format (default: %.2X%.2X%.2X);\n",
               (uint32_t)(config.bgColor.r),
               (uint32_t)(config.bgColor.g),
               (uint32_t)(config.bgColor.b));
        printf("  --             read null terminated files list from stdin.\n");
        printf("                 Usage:\n");
        printf("                   find /path -name *.psd -print0 | sviewgl --\n");

        printf("\nAvailable keys:\n");
        printf("  <esc> or <q>  exit;\n");
        printf("  <space>       next image;\n");
        printf("  <backspace>   previous image;\n");
        printf("  <+> / <->     scale image;\n");
        printf("  <1>...<0>     set scale from 100%% to 1000%%;\n");
        printf("  <pgdn>        next image in multi-page image;\n");
        printf("  <pgup>        previous image in multi-page image;\n");
        printf("  <enter>       switch fullscreen / windowed mode;\n");
        printf("  <del>         toggle deletion mark;\n");
        printf("  <ctrl>+<del>  delete marked images from disk;\n");
        printf("  <s>           fit image to window;\n");
        printf("  <r>           rotate clockwise;\n");
        printf("  <shift>+<r>   rotate counterclockwise;\n");
        printf("  <c>           change background index;\n");
        printf("  <i>           hide / show on-screen info;\n");
        printf("  <e>           hide / show exif popup;\n");
        printf("  <p>           hide / show pixel info;\n");
        printf("  <b>           hide / show border around image;\n");
        printf("  <g>           hide / show image grid;\n");
        printf("\n");
    }

    void callbackResize(GLFWwindow* /*window*/, int width, int height)
    {
        m_viewer->onResize({ width, height });
    }

    void callbackPosition(GLFWwindow* /*window*/, int x, int y)
    {
        m_viewer->onPosition({ x, y });
    }

    void callbackRedraw(GLFWwindow* /*window*/)
    {
        m_viewer->onRender();
    }

    void callbackKey(GLFWwindow* /*window*/, int key, int scancode, int action, int mods)
    {
        m_viewer->onKey(key, scancode, action, mods);
    }

    void callbackChar(GLFWwindow* /*window*/, unsigned int c)
    {
        m_viewer->onChar(c);
    }

    void callbackMouseButtons(GLFWwindow* /*window*/, int button, int action, int mods)
    {
        m_viewer->onMouseButtons(button, action, mods);
    }

    void callbackMousePosition(GLFWwindow* /*window*/, double x, double y)
    {
        m_viewer->onMouse({ (float)x, (float)y });
    }

    void callbackCursorEnter(GLFWwindow* /*window*/, int entered)
    {
        m_viewer->onCursorEnter(entered != 0);
    }

    void callbackMouseScroll(GLFWwindow* /*window*/, double x, double y)
    {
        m_viewer->onMouseScroll({ (float)x, (float)y });
    }

#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 1
    void callbackDrop(GLFWwindow* /*window*/, int count, const char** paths)
    {
        m_viewer->addPaths(paths, count);
    }
#endif

    void callbackError(int e, const char* error)
    {
        ::printf("(EE) GLFW error (%d) '%s'\n", e, error);
    }

    void setup(GLFWwindow* window)
    {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        glfwSetWindowSizeCallback(window, callbackResize);
        // glfwSetFramebufferSizeCallback(window, callbackResize);
        glfwSetWindowPosCallback(window, callbackPosition);

        glfwSetWindowRefreshCallback(window, callbackRedraw);

        glfwSetKeyCallback(window, callbackKey);
        glfwSetCharCallback(window, callbackChar);

        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
        glfwSetMouseButtonCallback(window, callbackMouseButtons);
        glfwSetCursorPosCallback(window, callbackMousePosition);
        glfwSetCursorEnterCallback(window, callbackCursorEnter);
        glfwSetScrollCallback(window, callbackMouseScroll);

        glfwSetErrorCallback(callbackError);

#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 1
        glfwSetDropCallback(window, callbackDrop);
#endif
    }

    GLFWwindow* createWindowedWindow(int width, int height, GLFWwindow* parent, const sConfig& config)
    {
        width = width <= 0 ? 640 : config.windowSize.x;
        height = height <= 0 ? 480 : config.windowSize.y;

        auto newWindow = glfwCreateWindow(width, height, SVGL_Title, nullptr, parent);

        if (config.centerWindow == false)
        {
            glfwSetWindowPos(newWindow, config.windowPos.x, config.windowPos.y);
        }

        return newWindow;
    }

    GLFWwindow* createFullscreenWindow(GLFWwindow* parent)
    {
        auto monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        auto newWindow = glfwCreateWindow(mode->width, mode->height, SVGL_Title, monitor, parent);
        return newWindow;
    }

    //
    // macOS Mojave / Xcode 10 specific bug workaround
    // https://github.com/glfw/glfw/issues/1334
    //
    static int macOSHackCount = 0;

    void macOSMojaveUglyHack(GLFWwindow* window)
    {
#ifdef __APPLE__
        if (macOSHackCount < 2)
        {
            macOSHackCount++;
            if (macOSHackCount == 1)
            {
                int x, y;
                glfwGetWindowPos(window, &x, &y);
                glfwSetWindowPos(window, ++x, y);
            }
            else if (macOSHackCount == 2)
            {
                int x, y;
                glfwGetWindowPos(window, &x, &y);
                glfwSetWindowPos(window, --x, y);
            }
        }
#endif
        (void)window;
    }

    class cOpenWrapper
    {
    public:
        void addFile(const char* filename)
        {
            if (filename != nullptr)
            {
                m_files.push_back(filename);
            }
        }

        using FilesList = std::vector<std::string>;

        const FilesList& getList() const
        {
            return m_files;
        }

    private:
        FilesList m_files;
    };

    cOpenWrapper OpenWrapper;

} // namespace

extern "C" {
void AddFile(const char* filename)
{
    if (m_viewer == nullptr)
    {
        OpenWrapper.addFile(filename);
    }
    else
    {
        const char* paths[] = { filename };
        m_viewer->addPaths(paths, 1);
    }
}

} // extern "C"

int main(int argc, char* argv[])
{
    ::setlocale(LC_ALL, "");

    sConfig config;

    for (int i = 1; i < argc; i++)
    {
        if (!::strncmp(argv[i], "-h", 2) || !::strncmp(argv[i], "--help", 6))
        {
            showVersion();
            showHelp(argv[0], config);
            return 0;
        }
        else if (!::strncmp(argv[i], "-v", 2) || !::strncmp(argv[i], "--version", 9))
        {
            showVersion();
            return 0;
        }
    }

    cConfig fileConfig;
    fileConfig.read(config);

    std::vector<const char*> pathsList;
    std::vector<std::string> stdinPaths;

    for (int i = 1; i < argc; i++)
    {
        if (::strncmp(argv[i], "--debug", 7) == 0)
        {
            config.debug = true;
        }
        else if (::strncmp(argv[i], "-mipmap", 7) == 0)
        {
            if (i + 1 < argc)
            {
                config.mipmapTextureSize = (uint32_t)::atoi(argv[++i]);
            }
        }
        else if (::strncmp(argv[i], "-svg", 4) == 0)
        {
            if (i + 1 < argc)
            {
                config.minSvgSize = (float)::atof(argv[++i]);
            }
        }
        else if (::strncmp(argv[i], "-cw", 3) == 0)
        {
            config.centerWindow = true;
        }
        else if (::strncmp(argv[i], "-wz", 3) == 0)
        {
            config.wheelZoom = true;
        }
        else if (::strncmp(argv[i], "-i", 2) == 0)
        {
            config.hideInfobar = true;
        }
        else if (::strncmp(argv[i], "-p", 2) == 0)
        {
            config.showPixelInfo = true;
        }
        else if (::strncmp(argv[i], "-e", 2) == 0)
        {
            config.showExif = true;
        }
        else if (::strncmp(argv[i], "-f", 2) == 0)
        {
            config.fullScreen = true;
        }
        else if (::strncmp(argv[i], "-c", 2) == 0)
        {
            config.backgroundIndex = 1;
        }
        else if (::strncmp(argv[i], "-s", 2) == 0)
        {
            config.fitImage = true;
        }
        else if (::strncmp(argv[i], "-b", 2) == 0)
        {
            config.showImageBorder = true;
        }
        else if (::strncmp(argv[i], "-g", 2) == 0)
        {
            config.showImageGrid = true;
        }
        else if (::strncmp(argv[i], "-r", 2) == 0)
        {
            config.recursiveScan = true;
        }
        else if (::strncmp(argv[i], "-a", 2) == 0)
        {
            config.skipFilter = true;
        }
        else if (::strncmp(argv[i], "-C", 2) == 0)
        {
            uint32_t r, g, b;
            if (3 == sscanf(argv[i + 1], "%2x%2x%2x", &r, &g, &b))
            {
                config.bgColor = { (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)255 };
                i++;
            }
        }
        else if (strcmp(argv[i], "--") == 0)
        {
            std::string path;
            while (1)
            {
                auto c = ::getc(stdin);
                if (c == EOF)
                {
                    break;
                }
                else if (c == 0)
                {
                    stdinPaths.push_back(path);
                    path.clear();
                }
                else
                {
                    path += c;
                }
            }
        }
        else
        {
            const char* path = argv[i];
            if (path[0] != '-')
            {
                pathsList.push_back(path);
            }
        }
    }

    cViewer viewer(config);
    m_viewer = &viewer;

    for (auto& p : stdinPaths)
    {
        pathsList.push_back(p.c_str());
    }

    for (auto& p : OpenWrapper.getList())
    {
        pathsList.push_back(p.c_str());
    }

    viewer.addPaths(pathsList.data(), pathsList.size());

    int result = 0;

    if (glfwInit())
    {
        GLFWwindow* window = nullptr;
        if (config.fullScreen)
        {
            window = createFullscreenWindow(nullptr);
            viewer.setWindowed(false);
        }
        else
        {
            window = createWindowedWindow(config.windowSize.x, config.windowSize.y, nullptr, config);

            viewer.setWindowed(true);
        }

        if (window != nullptr)
        {
            setup(window);
            viewer.setWindow(window);

            bool updateSizePos = false;

            while (!glfwWindowShouldClose(window))
            {
                macOSMojaveUglyHack(window);

                if (viewer.isWindowModeRequested())
                {
                    GLFWwindow* newWindow = nullptr;

                    const bool windowed = viewer.isWindowed();
                    if (windowed == false)
                    {
                        updateSizePos = true;

                        newWindow = createWindowedWindow(config.windowSize.x, config.windowSize.y, window, config);
                    }
                    else
                    {
                        viewer.setWindowed(false);

                        newWindow = createFullscreenWindow(window);
                    }

                    setup(newWindow);
                    viewer.setWindow(newWindow);

                    glfwDestroyWindow(window);
                    window = newWindow;

                    macOSHackCount = 0;
                }
                else if (updateSizePos)
                {
                    updateSizePos = false;
                    viewer.setWindowed(true);

                    glfwSetWindowSize(window, config.windowSize.x, config.windowSize.y);
                    glfwSetWindowPos(window, config.windowPos.x, config.windowPos.y);
                }

                const float timeStart = glfwGetTime();
                static float timeEnd = timeStart;

                viewer.onRender();
                viewer.onUpdate();

                glfwPollEvents();

                if (viewer.isUploading() == false)
                {
                    const float delta = timeEnd - timeStart;

                    const float disiredFps = 1.0f / 60.0f;
                    const float time_rest = disiredFps - delta;
                    if (time_rest > 0.0f)
                    {
                        usleep(time_rest * 1000000);
                    }
                }

                timeEnd = glfwGetTime();
            }

            fileConfig.write(config);
        }
        else
        {
            ::printf("(EE) Can't create window.\n");
            result = -1;
        }

        glfwTerminate();
    }
    else
    {
        ::printf("(EE) Can't initialize GLFW.\n");
        result = -1;
    }

    return result;
}
