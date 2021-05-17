/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "common/Ini.h"
#include "types/color.h"
#include "types/vector.h"

struct sConfig
{
    bool debug = false;

    bool hideInfobar = false;
    bool showPixelInfo = false;
    bool showExif = false;
    bool fitImage = false;
    bool showImageBorder = false;
    bool showImageGrid = false;
    bool recursiveScan = false;
    bool centerWindow = false;
    bool fullScreen = false;
    bool skipFilter = false;
    bool wheelZoom = true;
    bool keepScale = false;

    uint32_t mipmapTextureSize = 4096;
    uint32_t fileMaxLength = 100;

    uint32_t backgroundIndex = 0;
    cColor bgColor{ 90, 90, 90, 255 };
    uint32_t bgCellSize = 16;

    float fontRatio = 0.5f;

    float shiftInPixels = 10.0f;
    float shiftInPercent = 0.5f;

    float minSvgSize = 256.0f;

    Vectori windowSize{ 0, 0 };
    Vectori windowPos{ 0, 0 };
};

class cConfig final
{
public:
    cConfig();

    void read(sConfig& config) const;
    void write(const sConfig& config) const;

private:
    mutable ini::cIni m_ini;
};
