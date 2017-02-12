/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "helpers.h"

#include <cmath>
#include <ctime>

namespace helpers
{

    uint16_t read_uint16(uint8_t* p)
    {
        return (p[0] << 8) | p[1];
    }

    uint32_t read_uint32(uint8_t* p)
    {
        return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    }

    void swap_uint32s(uint8_t* p, uint32_t size)
    {
        const uint32_t items = size / 4;
        for (uint32_t i = 0; i < items; i++)
        {
            *((uint32_t*)p) = read_uint32(p);
            p += 4;
        }
    }

    void swap_uint16s(uint8_t* p, uint32_t size)
    {
        const uint32_t items = size / 2;
        for (uint32_t i = 0; i < items; i++)
        {
            *((uint16_t*)p) = read_uint16(p);
            p += 2;
        }
    }

    uint32_t nextPot(uint32_t n)
    {
        n = n - 1;
        n = n | (n >> 1);
        n = n | (n >> 2);
        n = n | (n >> 4);
        n = n | (n >> 8);
        n = n | (n >> 16);
        return n + 1;
    }

    uint32_t calculatePitch(uint32_t width, uint32_t bytesPP)
    {
        // texture pitch should be multiple by 4
        return (uint32_t)::ceilf(width * bytesPP / 4.0f) * 4;
    }

    uint64_t getTime()
    {
        timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        return now.tv_sec * 1000000000 + now.tv_nsec;
    }

}
