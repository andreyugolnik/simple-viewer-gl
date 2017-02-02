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

    void SetColor(int r, int g, int b, int a);
    void Render(float x, float y, float w, float h, float scale);
    float GetBorderWidth() const;

private:
    void renderLine(float x, float y, float w, float h);

private:
    sQuad m_line;
};
