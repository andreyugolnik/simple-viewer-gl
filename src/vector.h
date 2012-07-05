////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#ifndef VECTOR_H_E3551352CCAF74
#define VECTOR_H_E3551352CCAF74

#include <math.h>

// Fast 1.0/sqrtf(float) routine
float InvSqrt(float x);

class cVector
{
public:
    cVector() : x(0), y(0) { }
    cVector(float _x, float _y) : x(_x), y(_y) { }
    cVector(const cVector& _v) : x(_v.x), y(_v.y) { }

    cVector& operator= (const cVector& v) { x = v.x; y = v.y; return *this; }
    cVector operator- () const { return cVector(-x, -y); }
    cVector operator- (const cVector& v) const { return cVector(x-v.x, y-v.y); }
    cVector operator+ (const cVector& v) const { return cVector(x+v.x, y+v.y); }
    cVector& operator-= (const cVector& v) { x-=v.x; y-=v.y; return *this; }
    cVector& operator-= (float _val) { x -= _val; y -= _val; return *this; }
    cVector& operator+= (const cVector& v) { x+=v.x; y+=v.y; return *this; }
    cVector& operator+= (float _val) { x += _val; y += _val; return *this; }
    bool operator== (const cVector& v) const { return (x==v.x && y==v.y); }
    bool operator!= (const cVector& v) const { return (x!=v.x || y!=v.y); }

    cVector operator/ (const float scalar) const { return cVector(x/scalar, y/scalar); }
    cVector operator* (const float scalar) const { return cVector(x*scalar, y*scalar); }
    cVector& operator*= (const float scalar) { x*=scalar; y*=scalar; return *this; }

    float Dot(const cVector* v) const { return x * v->x + y * v->y; }
    float Length() const { return sqrtf(Dot(this)); }
    float Angle(const cVector* v = 0) const;
    void Clamp(const float max) { if(Length() > max) { Normalize(); x *= max; y *= max; } }
    cVector* Normalize() { float rc = InvSqrt(Dot(this)); x *= rc; y *= rc; return this; }
    cVector* Rotate(float a);

public:
    float x;
    float y;
};

inline cVector operator* (const float s, const cVector& v) { return v*s; }
inline float operator^ (const cVector& v, const cVector& u) { return v.Angle(&u); }
inline float operator% (const cVector& v, const cVector& u) { return v.Dot(&u); }

#endif /* end of include guard: VECTOR_H_E3551352CCAF74 */

