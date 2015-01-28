/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef VECTOR_H_E3551352CCAF74
#define VECTOR_H_E3551352CCAF74

template<typename T>
class cVector
{
public:
    cVector()
        : x(0)
        , y(0)
    {
    }
    cVector(T _x, T _y)
        : x(_x)
        , y(_y)
    {
    }
    cVector(const cVector& _v)
        : x(_v.x)
        , y(_v.y)
    {
    }

    cVector& operator= (const cVector& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }
    cVector operator- () const
    {
        return cVector(-x, -y);
    }
    cVector operator- (const cVector& v) const
    {
        return cVector(x - v.x, y - v.y);
    }
    cVector operator+ (const cVector& v) const
    {
        return cVector(x + v.x, y + v.y);
    }
    cVector& operator-= (const cVector& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    cVector& operator-= (T val)
    {
        x -= val;
        y -= val;
        return *this;
    }
    cVector& operator+= (const cVector& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    cVector& operator+= (T val)
    {
        x += val;
        y += val;
        return *this;
    }
    bool operator== (const cVector& v) const
    {
        return (x == v.x && y == v.y);
    }
    bool operator!= (const cVector& v) const
    {
        return (x != v.x || y != v.y);
    }

    cVector operator/ (const T scalar) const
    {
        return cVector(x / scalar, y / scalar);
    }
    cVector operator* (const T scalar) const
    {
        return cVector(x * scalar, y * scalar);
    }
    cVector& operator*= (const T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

public:
    T x;
    T y;
};

#endif /* end of include guard: VECTOR_H_E3551352CCAF74 */

