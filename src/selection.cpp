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

CSelection::CSelection() : m_visible(false), m_imageWidth(0), m_imageHeight(0) {
}

CSelection::~CSelection() {
}

void CSelection::SetImageDimension(int w, int h) {
	m_imageWidth	= w;
	m_imageHeight	= h;
}

void CSelection::StartPoint(int x, int y) {
	m_visible	= false;

	x	= std::max(x, 0);
	x	= std::min(x, m_imageWidth - 1);
	y	= std::max(y, 0);
	y	= std::min(y, m_imageHeight - 1);
	m_rc.SetLeftTop(x, y);
}

void CSelection::EndPoint(int x, int y) {
	x	= std::max(x, 0);
	x	= std::min(x, m_imageWidth);
	y	= std::max(y, 0);
	y	= std::min(y, m_imageHeight);
	m_rc.SetRightBottom(x, y);

	m_visible	= !(m_rc.GetWidth() == 0 || m_rc.GetHeight() == 0);
}

void CSelection::Render(int dx, int dy) {
	if(m_visible == true) {
		glColor4ub(0, 255, 0, 255);
		glLineWidth(1);
		glBindTexture(GL_TEXTURE_2D, 0);

		CRect rc;
		shiftRect(rc, dx, dy);

		renderLine(rc.x1, rc.y1, rc.x2, rc.y1);	// top line
		renderLine(rc.x1, rc.y1, rc.x1, rc.y2);	// left line
		renderLine(rc.x1, rc.y2, rc.x2, rc.y2);	// bottom line
		renderLine(rc.x2, rc.y1, rc.x2, rc.y2);	// right line
	}
}

void CSelection::renderLine(float x1, float y1, float x2, float y2) {
	glBegin(GL_LINES);
		glVertex2f(x1 + 0.5f, y1 + 0.5f); glVertex2f(x2 + 0.5f, y2 + 0.5f);
	glEnd();
}

void CSelection::shiftRect(CRect& rc, int dx, int dy) {
	rc.x1	= m_rc.x1 + dx;
	rc.x2	= m_rc.x2 + dx;
	rc.y1	= m_rc.y1 + dy;
	rc.y2	= m_rc.y2 + dy;
}

bool CSelection::GetRect(CRect& rc) {
	rc.Set(&m_rc);
	return m_visible;
}
