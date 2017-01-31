/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "buffer.h"

#include <cstdint>

class cFileInterface;

namespace helpers
{

    uint16_t read_uint16(uint8_t* p);
    uint32_t read_uint32(uint8_t* p);
    void swap_uint32s(uint8_t* p, uint32_t size);
    void swap_uint16s(uint8_t* p, uint32_t size);

    bool readBuffer(cFileInterface& file, Buffer& buffer, unsigned desired_size);

    template <class T, size_t N>
    constexpr size_t countof(const T(&)[N]) noexcept
    {
        return N;
    }

} // namespace helpers
