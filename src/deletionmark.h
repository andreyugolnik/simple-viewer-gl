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

class cDeletionMark final
{
public:
    void init();

    void render();

private:
    std::unique_ptr<cQuad> m_image;
};
