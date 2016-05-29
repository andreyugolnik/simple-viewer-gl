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

#if defined(LIBCONFIG_SUPPORT)
#include <libconfig.h++>
#endif

#include <cstdlib>
#include <cstdio>

void cConfig::read()
{
#if defined(LIBCONFIG_SUPPORT)
    libconfig::Config loader;

    char path[4096];

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
        loader.readFile(path);
    }
    catch(libconfig::FileIOException e)
    {
        printf("Can't open config.\n");
        return;
    }
    catch(libconfig::ParseException e)
    {
        printf("Can't parse config: %s in line %d\n", e.getError(), e.getLine());
        return;
    }
    catch(...)
    {
        printf("Error loading config.\n\n");
    }

    loader.lookupValue("hide_infobar", m_config.hideInfobar);
    loader.lookupValue("show_pixelinfo", m_config.showPixelInfo);
    loader.lookupValue("hide_checkboard", m_config.hideCheckboard);
    loader.lookupValue("fit_image", m_config.fitImage);
    //loader.lookupValue("start_fullscreen", m_config.startFullscreen);
    loader.lookupValue("show_image_border", m_config.showImageBorder);
    loader.lookupValue("lookup_recursive", m_config.recursiveScan);
    loader.lookupValue("center_window", m_config.centerWindow);
    loader.lookupValue("lookup_recursive", m_config.recursiveScan);
    loader.lookupValue("skip_filter", m_config.skipFilter);
    loader.lookupValue("wheel_zoom", m_config.wheelZoom);

    int value;
    if(loader.lookupValue("background_r", value))
    {
        m_config.color.r = value / 255.0f;
    }
    if(loader.lookupValue("background_g", value))
    {
        m_config.color.g = value / 255.0f;
    }
    if(loader.lookupValue("background_b", value))
    {
        m_config.color.b = value / 255.0f;
    }

#endif
}

