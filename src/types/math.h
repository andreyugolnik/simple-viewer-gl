/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

template<typename T>
T clamp(T min, T max, T val)
{
    return val < min ? min : (val > max ? max : val);
}
