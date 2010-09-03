/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef RECT_H
#define RECT_H

#include <math.h>

template<typename T>
class CRect {
public:
    CRect() : m_isSet(false), m_x1(0), m_y1(0), m_x2(0), m_y2(0) {
    }

    CRect(T x1, T y1, T x2, T y2) : m_isSet(true), m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2){
    }

    virtual ~CRect() {
    }

	void Clear() {
		m_isSet	= false;
	}

	bool IsSet() const {
		return m_isSet;
	}

	void Set(T x1, T y1, T x2, T y2) {
		SetLeftTop(x1, y1);
		SetRightBottom(x2, y2);
	}

	void SetLeftTop(T x, T y) {
		m_isSet	= true;
		m_x1	= x;
		m_y1	= y;
	}

	void SetRightBottom(T x, T y) {
		m_isSet	= true;
		m_x2	= x;
		m_y2	= y;
	}

	void Encapsulate(T x, T y) {
		if(m_isSet == false) {
			m_isSet	= true;
			m_x1 = x;
			m_x2 = x;
			m_y1 = y;
			m_y2 = y;
		}
		else {
			if(x < m_x1) m_x1 = x;
			if(x > m_x2) m_x2 = x;
			if(y < m_y1) m_y1 = y;
			if(y > m_y2) m_y2 = y;
		}
	}

	bool TestPoint(T x, T y) const {
		return (x >= m_x1 && x < m_x2 && y >= m_y1 && y < m_y2);
	}

	bool Intersect(const CRect* rc) const {
		if(fabs(m_x1 + m_x2 - rc->m_x1 - rc->m_x2) < (m_x2 - m_x1 + rc->m_x2 - rc->m_x1) &&
			fabs(m_y1 + m_y2 - rc->m_y1 - rc->m_y2) < (m_y2 - m_y1 + rc->m_y2 - rc->m_y1)) {

			return true;
		}
		return false;
	}

	T GetWidth() const {
		return static_cast<T>(fabs(m_x2 - m_x1));
	}

	T GetHeight() const {
		return static_cast<T>(fabs(m_y2 - m_y1));
	}

	CRect& operator=(const CRect<T>& rc) {
		if(&rc != this) {
			m_isSet	= rc.m_isSet;
			m_x1	= rc.m_x1;
			m_x2	= rc.m_x2;
			m_y1	= rc.m_y1;
			m_y2	= rc.m_y2;
		}
		return *this;
	}

private:
	bool m_isSet;

public:
	T m_x1, m_y1, m_x2, m_y2;
};

#endif // RECT_H
