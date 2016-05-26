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
class CRect final
{
public:
    CRect() :
        m_isSet(false), x1(0), y1(0), x2(0), y2(0)
    { }

    CRect(T _x1, T _y1, T _x2, T _y2) :
        m_isSet(true), x1(_x1), y1(_y1), x2(_x2), y2(_y2)
    { }

    virtual ~CRect()
    { }

    void Clear()
    {
        m_isSet = false;
    }

    bool IsSet() const
    {
        return m_isSet;
    }

    void Set(T x1, T y1, T x2, T y2)
    {
        SetLeftTop(x1, y1);
        SetRightBottom(x2, y2);
    }

    void SetLeftTop(T x, T y)
    {
        x1 = x;
        y1 = y;
        m_isSet = true;
    }

    void SetRightBottom(T x, T y)
    {
        x2 = x;
        y2 = y;
        m_isSet = true;
    }

    void ShiftRect(T dx, T dy)
    {
        x1 += dx;
        x2 += dx;
        y1 += dy;
        y2 += dy;
    }

    void Encapsulate(T x, T y)
    {
        if(m_isSet == false)
        {
            m_isSet = true;
            x1 = x;
            x2 = x;
            y1 = y;
            y2 = y;
        }
        else
        {
            if(x < x1) x1 = x;
            if(x > x2) x2 = x;
            if(y < y1) y1 = y;
            if(y > y2) y2 = y;
        }
    }

    bool TestPoint(T x, T y) const
    {
        return !(!m_isSet || x < x1 || x >= x2 || y < y1 || y >= y2);
    }

    bool Intersect(const CRect<T>* rc) const
    {
        if(fabs(x1 + x2 - rc->x1 - rc->x2) < (x2 - x1 + rc->x2 - rc->x1) &&
                fabs(y1 + y2 - rc->y1 - rc->y2) < (y2 - y1 + rc->y2 - rc->y1))
        {
            return true;
        }
        return false;
    }

    T GetWidth() const
    {
        if(x2 >= x1)
        {
            return x2 - x1;
        }
        return x1 - x2;
    }

    T GetHeight() const
    {
        if(y2 >= y1)
        {
            return y2 - y1;
        }
        return y1 - y2;
    }

    void Normalize()
    {
        if(x1 > x2)
        {
            T x = x2;
            x2 = x1;
            x1 = x;
        }
        if(y1 > y2)
        {
            T y = y2;
            y2 = y1;
            y1 = y;
        }
    }

    CRect& operator=(const CRect<T>& rc)
    {
        if(&rc != this)
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
    T x1, y1, x2, y2;
};

