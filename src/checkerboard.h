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

struct sConfig;

class cCheckerboard final
{
public:
    explicit cCheckerboard(const sConfig& config);
    void init();

    void render();

private:
    const sConfig& m_config;

    std::unique_ptr<cQuad> m_cb;
};
