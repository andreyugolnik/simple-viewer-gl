/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

struct sColor
{
    float r, g, b;
};

struct sConfig
{
    bool hideInfobar = false;
    bool showPixelInfo = false;
    bool hideCheckboard = false;
    bool fitImage = false;
    bool showImageBorder = false;
    bool recursiveScan = false;
    bool centerWindow = false;
    bool skipFilter = false;
    bool wheelZoom = false;
    bool debug = false;
    bool keepScale = false;

    sColor color{ 0.0f, 0.0f, 1.0f };
};

class cConfig final
{
public:
    void read();

    sConfig& getConfig()
    {
        return m_config;
    }

private:
    sConfig m_config;
};
