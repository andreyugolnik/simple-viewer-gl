/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "config.h"
#include "../viewer.h"
#include "Ini.h"

#include <cstdlib>
#include <strings.h>

namespace
{

    bool getBool(const char* value, bool def)
    {
        if (value != nullptr)
        {
            return strcasecmp(value, "1") == 0
                || strcasecmp(value, "true") == 0
                || strcasecmp(value, "enable") == 0;
        }
        return def;
    }

    int getInt(const char* value, int def)
    {
        return value != nullptr ? atoi(value) : def;
    }

}

void cConfig::read(sConfig& config) const
{
    char path[4096];

#if defined(__APPLE__)
    snprintf(path, sizeof(path), "%s/Library/sviewgl/config", getenv("HOME"));
#else
    // make config path according XDG spec
    const char* xdg_path = getenv("XDG_CONFIG_HOME");
    if (xdg_path != nullptr)
    {
        snprintf(path, sizeof(path), "%s/sviewgl/config", xdg_path);
    }
    else
    {
        snprintf(path, sizeof(path), "%s/.config/sviewgl/config", getenv("HOME"));
    }
#endif

    ini::cFile file;
    if (file.open(path) == false)
    {
        return;
    }

    // ::printf("Using config file: '%s'\n", path);

    ini::cIni ini;
    ini.read(&file);

    const char* section = "common";

    config.debug = getBool(ini.getString(section, "debug"), false);

    config.hideInfobar = getBool(ini.getString(section, "hide_infobar"), false);
    config.showPixelInfo = getBool(ini.getString(section, "show_pixelinfo"), false);
    config.hideCheckboard = getBool(ini.getString(section, "hide_checkboard"), false);
    config.fitImage = getBool(ini.getString(section, "fit_image"), false);
    config.showImageBorder = getBool(ini.getString(section, "show_image_border"), false);
    config.recursiveScan = getBool(ini.getString(section, "lookup_recursive"), false);
    config.centerWindow = getBool(ini.getString(section, "center_window"), false);
    config.skipFilter = getBool(ini.getString(section, "skip_filter"), false);
    config.wheelZoom = getBool(ini.getString(section, "wheel_zoom"), false);
    config.keepScale = getBool(ini.getString(section, "keep_scale"), false);

    config.color =
    {
        getInt(ini.getString(section, "background_r"),   0) / 255.0f,
        getInt(ini.getString(section, "background_g"),   0) / 255.0f,
        getInt(ini.getString(section, "background_b"), 255) / 255.0f,
    };
}
