/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "ftstring.h"
#include "quadseries.h"
#include "types/color.h"
#include "types/rect.h"
#include "types/vector.h"

#include <memory>

struct sPixelInfo
{
    sPixelInfo()
    {
        reset();
    }

    void reset()
    {
        mouse = { 0.0f, 0.0f };
        point = { 0.0f, 0.0f };
        imgWidth = 0;
        imgHeight = 0;
        bpp = 0;
        color = { 0, 0, 0, 0 };
        rc = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
    }

    Vectorf mouse;
    Vectorf point;
    uint32_t imgWidth;
    uint32_t imgHeight;
    uint32_t bpp;
    cColor color;
    Rectf rc;
};

class cPixelPopup final
{
public:
    void init();

    void setRatio(float ratio);

    void setPixelInfo(const sPixelInfo& pi);
    void render();
    void setCursor(int cursor);

private:
    void createFont();
    bool isInsideImage(const Vectorf& pos) const;

private:
    float m_ratio = 1.0f;
    float m_width = 0.0f;
    float m_height = 0.0f;
    sPixelInfo m_pixelInfo;
    std::string m_text;
    std::unique_ptr<cQuad> m_bg;
    std::unique_ptr<cQuadSeries> m_pointer;
    std::unique_ptr<cFTString> m_ft;
};
