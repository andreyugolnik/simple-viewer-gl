/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "renderer.h"

class cImageBorder final
{
public:
    cImageBorder();
    ~cImageBorder();

    void setColor(const cColor& color);
    void Render(float x, float y, float w, float h, float scale);
    float GetBorderWidth() const;

private:
    void renderLine(float x, float y, float w, float h);

private:
    sQuad m_line;
};
