/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cmath>

template<typename T>
class cRect final
{
public:
    cRect()
        : m_isSet(false)
        , x1(static_cast<T>(0))
        , y1(static_cast<T>(0))
        , x2(static_cast<T>(0))
        , y2(static_cast<T>(0))
    { }

    cRect(T X1, T Y1, T X2, T Y2)
        : m_isSet(true)
        , x1(X1)
        , y1(Y1)
        , x2(X2)
        , y2(Y2)
    { }

    void clear()
    {
        m_isSet = false;
    }

    bool isSet() const
    {
        return m_isSet;
    }

    void set(T x1, T y1, T x2, T y2)
    {
        setLeftTop(x1, y1);
        setRightBottom(x2, y2);
    }

    void setLeftTop(T x, T y)
    {
        x1 = x;
        y1 = y;
        m_isSet = true;
    }

    void setRightBottom(T x, T y)
    {
        x2 = x;
        y2 = y;
        m_isSet = true;
    }

    void shiftRect(T dx, T dy)
    {
        x1 += dx;
        x2 += dx;
        y1 += dy;
        y2 += dy;
    }

    void encapsulate(T x, T y)
    {
        if (m_isSet)
        {
            x1 = x < x1 ? x : x1;
            x2 = x > x2 ? x : x2;
            y1 = y < y1 ? y : y1;
            y2 = y > y2 ? y : y2;
        }
        else
        {
            m_isSet = true;
            x1 = x;
            x2 = x;
            y1 = y;
            y2 = y;
        }
    }

    bool testPoint(T x, T y) const
    {
        return !(!m_isSet || x < x1 || x >= x2 || y < y1 || y >= y2);
    }

    bool intersect(const cRect<T>* rc) const
    {
        if (fabs(x1 + x2 - rc->x1 - rc->x2) < (x2 - x1 + rc->x2 - rc->x1) &&
            fabs(y1 + y2 - rc->y1 - rc->y2) < (y2 - y1 + rc->y2 - rc->y1))
        {
            return true;
        }
        return false;
    }

    T width() const
    {
        return (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    }

    T height() const
    {
        return (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    }

    void normalize()
    {
        if (x1 > x2)
        {
            auto x = x2;
            x2 = x1;
            x1 = x;
        }
        if (y1 > y2)
        {
            auto y = y2;
            y2 = y1;
            y1 = y;
        }
    }

    cRect& operator=(const cRect<T>& rc)
    {
        if (&rc != this)
        {
            m_isSet = rc.m_isSet;
            x1 = rc.x1;
            x2 = rc.x2;
            y1 = rc.y1;
            y2 = rc.y2;
        }
        return *this;
    }

private:
    bool m_isSet;

public:
    T x1;
    T y1;
    T x2;
    T y2;
};

typedef cRect<float> Rectf;
typedef cRect<int> Recti;
