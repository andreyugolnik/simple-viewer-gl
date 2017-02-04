/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../types/color.h"

struct sConfig
{
    bool debug = false;

    bool hideInfobar = false;
    bool showPixelInfo = false;
    bool showExif = false;
    bool hideCheckboard = false;
    bool fitImage = false;
    bool showImageBorder = false;
    bool recursiveScan = false;
    bool centerWindow = false;
    bool skipFilter = false;
    bool wheelZoom = false;
    bool keepScale = false;

    uint32_t mipmapTextureSize = 4096;

    cColor bgColor{ 0, 0, 255, 255 };
};

class cConfig final
{
public:
    void read(sConfig& config) const;
};
