/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "config.h"
#include "Ini.h"
#include "viewer.h"

#include <cstdio>
#include <cstdlib>
#include <strings.h>

namespace
{
    template<typename T>
    T getValue(const char* /*value*/, T def)
    {
        return def;
    }

    template<>
    bool getValue<bool>(const char* value, bool def)
    {
        if (value != nullptr)
        {
            return ::strcasecmp(value, "1") == 0
                || ::strcasecmp(value, "true") == 0
                || ::strcasecmp(value, "enable") == 0;
        }
        return def;
    }

    template<>
    uint32_t getValue<uint32_t>(const char* value, uint32_t def)
    {
        return value != nullptr ? (uint32_t)::atoi(value) : def;
    }

    template<>
    uint8_t getValue<uint8_t>(const char* value, uint8_t def)
    {
        return value != nullptr ? (uint8_t)::atoi(value) : def;
    }

    template<>
    float getValue<float>(const char* value, float def)
    {
        return value != nullptr ? ::atof(value) : def;
    }

    const char* SectionName = "common";

    template<typename T>
    void readValue(const ini::cIni& ini, const char* name, T& value)
    {
        const auto def = value;
        value = getValue(ini.getString(SectionName, name), def);
    }
}

void cConfig::read(sConfig& config) const
{
    char path[4096];

#if defined(__APPLE__)
    ::snprintf(path, sizeof(path), "%s/Library/Application Support/sviewgl/config", ::getenv("HOME"));
#else
    // make config path according XDG spec
    const char* xdg_path = ::getenv("XDG_CONFIG_HOME");
    if (xdg_path != nullptr)
    {
        ::snprintf(path, sizeof(path), "%s/sviewgl/config", xdg_path);
    }
    else
    {
        ::snprintf(path, sizeof(path), "%s/.config/sviewgl/config", ::getenv("HOME"));
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

    readValue(ini, "debug", config.debug);

    readValue(ini, "hide_infobar", config.hideInfobar);
    readValue(ini, "show_pixelinfo", config.showPixelInfo);
    readValue(ini, "show_exif", config.showExif);
    readValue(ini, "hide_checkboard", config.hideCheckboard);
    readValue(ini, "fit_image", config.fitImage);
    readValue(ini, "show_image_border", config.showImageBorder);
    readValue(ini, "show_image_grid", config.showImageGrid);
    readValue(ini, "lookup_recursive", config.recursiveScan);
    readValue(ini, "center_window", config.centerWindow);
    readValue(ini, "full_screen", config.fullScreen);
    readValue(ini, "skip_filter", config.skipFilter);
    readValue(ini, "wheel_zoom", config.wheelZoom);
    readValue(ini, "keep_scale", config.keepScale);

    readValue(ini, "mipmap_texture_size", config.mipmapTextureSize);
    readValue(ini, "file_max_length", config.fileMaxLength);

    readValue(ini, "background_r", config.bgColor.r);
    readValue(ini, "background_g", config.bgColor.g);
    readValue(ini, "background_b", config.bgColor.b);

    readValue(ini, "background_cell_size", config.bgCellSize);

    readValue(ini, "font_ratio", config.fontRatio);
}
