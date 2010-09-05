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

CSelection::CSelection() : m_enabled(true), m_imageWidth(0), m_imageHeight(0) {
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

			const unsigned char color	= (checker_width_odd == true ? 0x00 : 0xff);
			*p++	= color;
			*p++	= color;
			*p++	= color;
		}
	}

	m_selection.reset(new CQuad(TEX_SIZE, TEX_SIZE, buffer, GL_RGB));
	m_selection->SetColor(200, 255, 200, 150);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	delete[] buffer;
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
		CRect<int> rc;
		setImagePos(rc, dx, dy);

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
	int x	= std::min(x1, x2);
	int y	= std::min(y1, y2);
	int w	= (x1 == x2 ? 1 : m_rc.GetWidth());
	int h	= (y1 == y2 ? 1 : m_rc.GetHeight());

	m_selection->SetSpriteSize(w, h);
	m_selection->Render(x, y);
}

void CSelection::setImagePos(CRect<int>& rc, int dx, int dy) {
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
