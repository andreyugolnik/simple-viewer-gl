/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef HELPERS_H_H36983FUY3543BCD
#define HELPERS_H_H36983FUY3543BCD

#include <cstdint>
#include <vector>

class cFileInterface;
typedef std::vector<unsigned char> Buffer;

uint16_t read_uint16(uint8_t* p);
uint32_t read_uint32(uint8_t* p);
void swap_uint32s(uint8_t* p, uint32_t size);
bool readBuffer(cFileInterface& file, Buffer& buffer, unsigned desired_size);

#endif // HELPERS_H_H36983FUY3543BCD

