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
class cVector final
{
public:
    cVector()
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
    {
    }
    cVector(T X, T Y)
        : x(X)
        , y(Y)
    {
    }
    cVector(const cVector& v)
        : x(v.x)
        , y(v.y)
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

    cVector operator- (T scalar) const
    {
        return cVector(x - scalar, y - scalar);
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

    cVector operator/ (T scalar) const
    {
        return cVector(x / scalar, y / scalar);
    }

    cVector operator* (T scalar) const
    {
        return cVector(x * scalar, y * scalar);
    }

    cVector operator* (const cVector& v) const
    {
        return cVector(x * v.x, y * v.y);
    }

    cVector& operator*= (const cVector& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    cVector& operator*= (T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

public:
    T x;
    T y;
};

typedef cVector<float> Vectorf;
typedef cVector<int> Vectori;
