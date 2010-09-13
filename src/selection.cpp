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

#include <algorithm>

const int TEX_SIZE	= 16;

CSelection::CSelection() : m_enabled(true), m_imageWidth(0), m_imageHeight(0), m_mouseInside(false), m_mouseMode(MODE_MOVE) {
}

CSelection::~CSelection() {
}

void CSelection::Init() {
	unsigned char* buffer	= new unsigned char[TEX_SIZE * TEX_SIZE * 3];
	unsigned char* p	= buffer;
	bool checker_height_odd	= true;
	for(int y = 0; y < TEX_SIZE; y++) {
		if(y % 4 == 0) {
			checker_height_odd	= !checker_height_odd;
		}

		bool checker_width_odd	= checker_height_odd;
		for(int x = 0; x < TEX_SIZE; x++) {
			if(x % 4 == 0) {
				checker_width_odd	= !checker_width_odd;
			}

			const unsigned char color	= (checker_width_odd == true ? 0x20 : 0xff);
			*p++	= color;
			*p++	= color;
			*p++	= color;
		}
	}

	m_selection.reset(new CQuad(TEX_SIZE, TEX_SIZE, buffer, GL_RGB));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	delete[] buffer;
}

void CSelection::SetImageDimension(int w, int h) {
	m_imageWidth	= w;
	m_imageHeight	= h;
	m_mouseInside	= false;
	m_rc.Clear();
}

void CSelection::MouseDown(int x, int y) {
	m_rc.Normalize();
	m_mouseInside	= m_rc.TestPoint(x, y);
	if(m_mouseInside == false) {
		clampPoint(x, y);
		m_rc.SetLeftTop(x, y);
		m_rc.Clear();
		m_mouseMode	= MODE_CROSS;
	}
	else {
		// calculate mouse mode
		const int delta	= 10;

		CRect<int> rcLe(m_rc.x1, m_rc.y1, m_rc.x1 + delta, m_rc.y2);
		CRect<int> rcRi(m_rc.x2 - delta, m_rc.y1, m_rc.x2, m_rc.y2);
		CRect<int> rcUp(m_rc.x1, m_rc.y1, m_rc.x2, m_rc.y1 + delta);
		CRect<int> rcDn(m_rc.x1, m_rc.y2 - delta, m_rc.x2, m_rc.y2);

		if(rcLe.TestPoint(x, y) == true) {
			if(rcUp.TestPoint(x, y) == true) {
				m_mouseMode	= MODE_LEUP;
			}
			else if(rcDn.TestPoint(x, y) == true) {
				m_mouseMode	= MODE_LEDN;
			}
			else {
				m_mouseMode	= MODE_LEFT;
			}
		}
		else if(rcRi.TestPoint(x, y) == true) {
			if(rcUp.TestPoint(x, y) == true) {
				m_mouseMode	= MODE_RIUP;
			}
			else if(rcDn.TestPoint(x, y) == true) {
				m_mouseMode	= MODE_RIDN;
			}
			else {
				m_mouseMode	= MODE_RIGHT;
			}
		}
		else if(rcUp.TestPoint(x, y) == true) {
			m_mouseMode	= MODE_UP;
		}
		else if(rcDn.TestPoint(x, y) == true) {
			m_mouseMode	= MODE_DOWN;
		}
		else {
			m_mouseMode	= MODE_MOVE;
		}

		m_mouseX	= x;
		m_mouseY	= y;
	}
}

void CSelection::MouseMove(int x, int y) {
	if(m_mouseInside == true) {
		int dx	= x - m_mouseX;
		int dy	= y - m_mouseY;

		// correct selection position
		dx	= std::max(dx, 0 - m_rc.x1);
		dx	= std::min(dx, m_imageWidth - (m_rc.x1 + m_rc.GetWidth()) - 1);
		dy	= std::max(dy, 0 - m_rc.y1);
		dy	= std::min(dy, m_imageHeight - (m_rc.y1 + m_rc.GetHeight()) - 1);

		switch(m_mouseMode) {
		case MODE_MOVE:
			m_rc.ShiftRect(dx, dy);
			break;
		case MODE_LEFT:
			m_rc.x1	+= dx;
			break;
		case MODE_RIGHT:
			m_rc.x2	+= dx;
			break;
		case MODE_UP:
			m_rc.y1	+= dy;
			break;
		case MODE_DOWN:
			m_rc.y2	+= dy;
			break;
		case MODE_LEUP:
			m_rc.x1	+= dx;
			m_rc.y1	+= dy;
			break;
		case MODE_RIUP:
			m_rc.x2	+= dx;
			m_rc.y1	+= dy;
			break;
		case MODE_LEDN:
			m_rc.x1	+= dx;
			m_rc.y2	+= dy;
			break;
		case MODE_RIDN:
			m_rc.x2	+= dx;
			m_rc.y2	+= dy;
			break;
		}

		m_mouseX	= x;
		m_mouseY	= y;
	}
	else {
		clampPoint(x, y);
		m_rc.SetRightBottom(x, y);
	}
}

void CSelection::Render(int dx, int dy) {
	if(m_enabled == true && m_rc.IsSet() == true) {
		CRect<int> rc;
		setImagePos(rc, dx, dy);

		setColor(m_mouseMode != MODE_UP);
		renderLine(rc.x1, rc.y1, rc.x2, rc.y1);	// top line
		setColor(m_mouseMode != MODE_DOWN);
		renderLine(rc.x1, rc.y2, rc.x2, rc.y2);	// bottom line
		setColor(m_mouseMode != MODE_LEFT);
		renderLine(rc.x1, rc.y1, rc.x1, rc.y2);	// left line
		setColor(m_mouseMode != MODE_RIGHT);
		renderLine(rc.x2, rc.y1, rc.x2, rc.y2);	// right line
	}
}

CRect<int> CSelection::GetRect() const {
	return m_rc;
}

int CSelection::GetCursor() const {
	return m_mouseMode;
}

void CSelection::renderLine(int x1, int y1, int x2, int y2) {
	int x	= std::min(x1, x2);
	int y	= std::min(y1, y2);
	int w	= (x1 == x2 ? 1 : m_rc.GetWidth());
	int h	= (y1 == y2 ? 1 : m_rc.GetHeight());

	m_selection->SetSpriteSize(w, h);
	m_selection->Render(x, y);
}

void CSelection::setImagePos(CRect<int>& rc, int dx, int dy) {
	rc.x1	= m_rc.x1 + dx;
	rc.x2	= m_rc.x2 + dx;
	rc.y1	= m_rc.y1 + dy;
	rc.y2	= m_rc.y2 + dy;
}

void CSelection::clampPoint(int& x, int& y) {
	x	= std::max(x, 0);
	x	= std::min(x, m_imageWidth - 1);
	y	= std::max(y, 0);
	y	= std::min(y, m_imageHeight - 1);
}

void CSelection::setColor(bool std) {
	if(std == true) {
		m_selection->SetColor(200, 255, 200, 150);
	}
	else {
		m_selection->SetColor(255, 255, 0, 255);
	}
}
