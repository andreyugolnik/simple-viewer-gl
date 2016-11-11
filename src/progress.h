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
#include <memory>

class CProgress final
{
public:
    void Init();
    void Start();
    void Render(float percent);

private:
    int m_loadingTime = 0;
    std::unique_ptr<CQuad> m_loading;
    std::unique_ptr<CQuad> m_square;
};

