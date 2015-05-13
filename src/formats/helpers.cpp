/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "helpers.h"
#include "file.h"

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
    for(uint32_t i = 0; i < items; i++)
    {
        *((uint32_t*)p) = read_uint32(p);
        p += 4;
    }
}

void swap_uint16s(uint8_t* p, uint32_t size)
{
    const uint32_t items = size / 2;
    for(uint32_t i = 0; i < items; i++)
    {
        *((uint16_t*)p) = read_uint16(p);
        p += 2;
    }
}

bool readBuffer(cFileInterface& file, Buffer& buffer, unsigned desired_size)
{
    const unsigned size = buffer.size();
    if(size < desired_size)
    {
        buffer.resize(desired_size);
        const unsigned length = desired_size - size;
        if(length != file.read(&buffer[size], length))
        {
            return false;
        }
    }
    return desired_size <= buffer.size();
}

