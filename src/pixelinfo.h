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
#include "math/vector.h"
#include "math/rect.h"

#include <memory>

struct sPixelInfo
{
    void reset()
    {
        mouse = { 0.0f, 0.0f };
        point = { 0.0f, 0.0f };
        img_w = 0;
        img_h = 0;
        r = 0;
        g = 0;
        b = 0;
        a = 0;

        rc = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    cVector<float> mouse;
    cVector<float> point;
    int img_w;
    int img_h;
    unsigned char r, g, b, a;
    CRect<float> rc;
};

class CPixelInfo final
{
public:
    void Init();

    void setRatio(float ratio);

    void setPixelInfo(const sPixelInfo& pi);
    void Render();
    void SetCursor(int cursor);

private:
    void createFont();
    bool isInsideImage(const cVector<float>& pos) const;

private:
    float m_ratio = 1.0f;
    sPixelInfo m_pixelInfo;
    std::unique_ptr<CQuad> m_bg;
    std::unique_ptr<CQuadSeries> m_pointer;
    std::unique_ptr<CFTString> m_ft;
};

