/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "rect.h"
#include <math.h>

CRect::CRect() : m_isSet(false), m_x1(0), m_y1(0), m_x2(0), m_y2(0) {
}

CRect::CRect(float x1, float y1, float x2, float y2) : m_isSet(true), m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2) {
}

CRect::~CRect() {
}

void CRect::Set(float x1, float y1, float x2, float y2) {
	m_isSet	= true;
	SetLeftTop(x1, y1);
	SetRightBottom(x2, y2);
}

void CRect::Set(CRect* rc) {
	if(rc != this) {
		rc->Set(m_x1, m_y1, m_x2, m_y2);
	}
}

void CRect::SetLeftTop(float x, float y) {
	m_x1	= x;
	m_y1	= y;
}

void CRect::SetRightBottom(float x, float y) {
	m_x2	= x;
	m_y2	= y;
}

void CRect::Encapsulate(float x, float y) {
	m_isSet	= true;
	if(x < m_x1) m_x1 = x;
	if(x > m_x2) m_x2 = x;
	if(y < m_y1) m_y1 = y;
	if(y > m_y2) m_y2 = y;
}

bool CRect::TestPoint(float x, float y) const {
    if(x >= m_x1 && x < m_x2 && y >= m_y1 && y < m_y2) {
		return true;
    }
    return false;
}

bool CRect::Intersect(const CRect* rc) const {
	if(fabs(m_x1 + m_x2 - rc->m_x1 - rc->m_x2) < (m_x2 - m_x1 + rc->m_x2 - rc->m_x1) &&
		fabs(m_y1 + m_y2 - rc->m_y1 - rc->m_y2) < (m_y2 - m_y1 + rc->m_y2 - rc->m_y1)) {

		return true;
	}
    return false;
}

float CRect::GetWidth() const {
	return fabs(m_x2 - m_x1);
}

float CRect::GetHeight() const {
	return fabs(m_y2 - m_y1);
}

