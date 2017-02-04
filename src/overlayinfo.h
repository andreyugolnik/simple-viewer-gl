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
#include "math/vector.h"
#include "math/rect.h"
#include "quadseries.h"

#include <memory>

class cOverlayInfo final
{
public:
    void init();
    void setRatio(float ratio);

    void setData(const char* data);
    void render();

private:
    void createFont();

private:
    float m_ratio = 1.0f;
    unsigned m_rows = 0;
    std::unique_ptr<cQuad> m_bg;
    std::unique_ptr<cFTString> m_ft;
};
