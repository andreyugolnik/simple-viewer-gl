/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../types/types.h"

namespace helpers
{

    uint16_t read_uint16(uint8_t* p);
    uint32_t read_uint32(uint8_t* p);
    void swap_uint32s(uint8_t* p, uint32_t size);
    void swap_uint16s(uint8_t* p, uint32_t size);

    template <class T, size_t N>
    constexpr size_t countof(const T(&)[N]) noexcept
    {
        return N;
    }

    uint32_t nextPot(uint32_t n);
    uint32_t calculatePitch(uint32_t width, uint32_t bytesPP);

    uint64_t getTime();

}
