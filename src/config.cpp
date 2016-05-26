/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "config.h"
#include "viewer.h"

#if !defined(PATH_MAX)
#define PATH_MAX 4096 /* # chars in a path name including nul */
#endif

#if defined(LIBCONFIG_SUPPORT)
#include <libconfig.h++>
#endif

#include <cstdlib>
#include <cstdio>

CConfig::CConfig(cViewer* viewer)
    : m_viewer(viewer)
    , m_opened(false)
{
#if defined(LIBCONFIG_SUPPORT)
    m_config.reset(new libconfig::Config());
#endif
}

CConfig::~CConfig()
{
}

bool CConfig::Open()
{
    m_opened = false;

#if defined(LIBCONFIG_SUPPORT)
    char path[PATH_MAX];

#if defined(__APPLE__)
    snprintf(path, sizeof(path), "%s/Library/sviewgl/config", getenv("HOME"));
#else
    // make config path according XDG spec
    const char* xdg_path = getenv("XDG_CONFIG_HOME");
    if(xdg_path)
    {
        snprintf(path, sizeof(path), "%s/sviewgl/config", xdg_path);
    }
    else
    {
        snprintf(path, sizeof(path), "%s/.config/sviewgl/config", getenv("HOME"));
    }
#endif

    printf("Using config file: '%s'\n", path);

    // try to read config
    try
    {
        m_config->readFile(path);
    }
    catch(libconfig::FileIOException e)
    {
        printf("Can't open config.\n");
        return false;
    }
    catch(libconfig::ParseException e)
    {
        printf("Can't parse config: %s in line %d\n", e.getError(), e.getLine());
        return false;
    }

    m_opened = true;
#endif

    return true;
}

void CConfig::Read()
{
#if defined(LIBCONFIG_SUPPORT)
    if(m_opened == true)
    {
        bool value;

        if(true == m_config->lookupValue("hide_infobar", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::Infobar);
        }

        if(true == m_config->lookupValue("show_pixelinfo", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::PixelInfo);
        }

        if(true == m_config->lookupValue("hide_checkboard", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::Checkers);
        }

        if(true == m_config->lookupValue("fit_image", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::FitImage);
        }

        if(true == m_config->lookupValue("start_fullscreen", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::Fullscreen);
        }

        if(true == m_config->lookupValue("show_image_border", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::Border);
        }

        if(true == m_config->lookupValue("lookup_recursive", value) && value == true) {
            m_viewer->SetProp(cViewer::Property::Recursive);
        }

        int r, g, b;
        if(m_config->lookupValue("background_r", r)
                && m_config->lookupValue("background_g", g)
                && m_config->lookupValue("background_b", b))
        {
            m_viewer->SetProp(r, g, b);
        }
    }
#endif
}

