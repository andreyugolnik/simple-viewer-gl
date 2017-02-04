/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types.h"

struct cColor final
{
public:
    cColor() = default;

    cColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
        : r(R)
        , g(G)
        , b(B)
        , a(A)
    {
    }

public:
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;
};
