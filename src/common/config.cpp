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

#include <cstdio>
#include <cstdlib>
#include <strings.h>

namespace
{
    template <typename T>
    T getValue(const char* /*value*/, T def)
    {
        ::printf("(EE) Unknown config value type!\n");
        return def;
    }

    template <>
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

    template <>
    int getValue<int>(const char* value, int def)
    {
        return value != nullptr ? ::atoi(value) : def;
    }

    template <>
    uint32_t getValue<uint32_t>(const char* value, uint32_t def)
    {
        return value != nullptr ? (uint32_t)::atoi(value) : def;
    }

    template <>
    uint8_t getValue<uint8_t>(const char* value, uint8_t def)
    {
        return value != nullptr ? (uint8_t)::atoi(value) : def;
    }

    template <>
    float getValue<float>(const char* value, float def)
    {
        return value != nullptr ? ::atof(value) : def;
    }

    const char* SectionName = "common";

    template <typename T>
    void readValue(const ini::cIni& ini, const char* name, T& value)
    {
        const auto def = value;
        value = getValue(ini.getString(SectionName, name), def);
    }
} // namespace

cConfig::cConfig()
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

    m_path = path;
}

void cConfig::read(sConfig& config) const
{
    auto path = m_path.c_str();

    ini::cFile file;
    if (file.open(path) == false)
    {
        return;
    }

    // ::printf("Using config file: '%s'\n", path);

    m_ini.read(&file);

    readValue(m_ini, "debug", config.debug);

    readValue(m_ini, "hide_infobar", config.hideInfobar);
    readValue(m_ini, "show_pixelinfo", config.showPixelInfo);
    readValue(m_ini, "show_exif", config.showExif);
    readValue(m_ini, "hide_checkboard", config.hideCheckboard);
    readValue(m_ini, "fit_image", config.fitImage);
    readValue(m_ini, "show_image_border", config.showImageBorder);
    readValue(m_ini, "show_image_grid", config.showImageGrid);
    readValue(m_ini, "lookup_recursive", config.recursiveScan);
    readValue(m_ini, "center_window", config.centerWindow);
    readValue(m_ini, "full_screen", config.fullScreen);
    readValue(m_ini, "skip_filter", config.skipFilter);
    readValue(m_ini, "wheel_zoom", config.wheelZoom);
    readValue(m_ini, "keep_scale", config.keepScale);

    readValue(m_ini, "mipmap_texture_size", config.mipmapTextureSize);
    readValue(m_ini, "file_max_length", config.fileMaxLength);

    readValue(m_ini, "background_r", config.bgColor.r);
    readValue(m_ini, "background_g", config.bgColor.g);
    readValue(m_ini, "background_b", config.bgColor.b);

    readValue(m_ini, "background_cell_size", config.bgCellSize);

    readValue(m_ini, "font_ratio", config.fontRatio);

    readValue(m_ini, "shift_in_pixels", config.shiftInPixels);
    readValue(m_ini, "shift_in_percent", config.shiftInPercent);

    readValue(m_ini, "window_x", config.windowPos.x);
    readValue(m_ini, "window_y", config.windowPos.y);

    readValue(m_ini, "window_w", config.windowSize.x);
    readValue(m_ini, "window_h", config.windowSize.y);
}

void cConfig::write(const sConfig& config) const
{
    auto path = m_path.c_str();

    ini::cFile file;
    if (file.open(path, "w") == false)
    {
        return;
    }

    m_ini.setString(SectionName, "window_x", std::to_string(config.windowPos.x).c_str());
    m_ini.setString(SectionName, "window_y", std::to_string(config.windowPos.y).c_str());

    m_ini.setString(SectionName, "window_w", std::to_string(config.windowSize.x).c_str());
    m_ini.setString(SectionName, "window_h", std::to_string(config.windowSize.y).c_str());

    m_ini.save(&file);
}
