////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#include "vector.h"

float InvSqrt(float x)
{
    union
    {
        int intPart;
        float floatPart;
    } convertor;

    convertor.floatPart	= x;
    convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
    return convertor.floatPart*(1.5f - 0.4999f * x * convertor.floatPart * convertor.floatPart);
}

float cVector::Angle(const cVector* v) const
{
    if(v)
    {
        cVector s = *this;
        cVector t = *v;

        s.Normalize();
        t.Normalize();
        return acosf(s.Dot(&t));
    }
    else
    {
        return atan2f(y, x);
    }
}

cVector* cVector::Rotate(float a)
{
    cVector v;

    v.x = x * cosf(a) - y * sinf(a);
    v.y = x * sinf(a) + y * cosf(a);

    x = v.x;
    y = v.y;

    return this;
}

