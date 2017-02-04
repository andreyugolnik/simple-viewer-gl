/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "quad.h"
#include "types/color.h"

#include <memory>

class cCheckerboard final
{
public:
    void init();

    void render(bool checkboardEanbled);
    void setColor(const cColor& color)
    {
        m_color = color;
    }

private:
    cColor m_color = { 0, 0, 255, 255 };
    std::unique_ptr<cQuad> m_cb;
};
