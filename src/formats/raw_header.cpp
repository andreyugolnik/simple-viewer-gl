/**********************************************\
*
*  AGE by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "raw_header.h"

#include <cstring>

namespace RAW
{

const char RawId[] = { 'R', 'w', '0', '1' };

void filEmpty(Header& header)
{
    memcpy(&header.id, RawId, sizeof(header.id));
}

bool isRawHeader(const Header& header)
{
    return memcmp(&header.id, RawId, sizeof(header.id)) == 0;
}

}
