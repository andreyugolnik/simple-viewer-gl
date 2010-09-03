////////////////////////////////////////////////
//
// Simple Viewer GL edition
// Andrey A. Ugolnik
// http://www.wegroup.org
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#include "selection.h"

#include <GL/gl.h>
#include <algorithm>

CSelection::CSelection() : m_enabled(true), m_imageWidth(0), m_imageHeight(0) {
}

CSelection::~CSelection() {
}

void CSelection::SetImageDimension(int w, int h) {
	m_imageWidth	= w;
	m_imageHeight	= h;
	m_rc.Clear();
}

void CSelection::StartPoint(int x, int y) {
	clampPoint(x, y);
	m_rc.SetLeftTop(x, y);
	m_rc.Clear();
}

void CSelection::EndPoint(int x, int y) {
	clampPoint(x, y);
	m_rc.SetRightBottom(x, y);
}

void CSelection::Render(int dx, int dy) {
	if(m_enabled == true && m_rc.IsSet() == true) {
		glColor4ub(0, 255, 0, 255);
		glLineWidth(1);
		glBindTexture(GL_TEXTURE_2D, 0);

		CRect<int> rc;
		shiftRect(rc, dx, dy);

		renderLine(rc.m_x1, rc.m_y1, rc.m_x2, rc.m_y1);	// top line
		renderLine(rc.m_x1, rc.m_y2, rc.m_x2, rc.m_y2);	// bottom line
		renderLine(rc.m_x1, rc.m_y1, rc.m_x1, rc.m_y2);	// left line
		renderLine(rc.m_x2, rc.m_y1, rc.m_x2, rc.m_y2);	// right line
	}
}

CRect<int> CSelection::GetRect() const {
	return m_rc;
}

void CSelection::renderLine(int x1, int y1, int x2, int y2) {
	glBegin(GL_LINES);
		glVertex2f(x1 + 0.5f, y1 + 0.5f); glVertex2f(x2 + 0.5f, y2 + 0.5f);
	glEnd();
}

void CSelection::shiftRect(CRect<int>& rc, int dx, int dy) {
	rc.m_x1	= m_rc.m_x1 + dx;
	rc.m_x2	= m_rc.m_x2 + dx;
	rc.m_y1	= m_rc.m_y1 + dy;
	rc.m_y2	= m_rc.m_y2 + dy;
}

void CSelection::clampPoint(int& x, int& y) {
	x	= std::max(x, 0);
	x	= std::min(x, m_imageWidth - 1);
	y	= std::max(y, 0);
	y	= std::min(y, m_imageHeight - 1);
}
