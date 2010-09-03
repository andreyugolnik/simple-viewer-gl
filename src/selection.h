////////////////////////////////////////////////
//
// Simple Viewer GL edition
// Andrey A. Ugolnik
// http://www.wegroup.org
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#ifndef SELECTION_H
#define SELECTION_H

#include "rect.h"

class CSelection {
public:
	CSelection();
	virtual ~CSelection();

	void SetImageDimension(int w, int h);
	void StartPoint(int x, int y);
	void EndPoint(int x, int y);
	void Render(int dx, int dy);
	bool GetRect(CRect& rc);

private:
	bool m_visible;
	int m_imageWidth, m_imageHeight;
	CRect m_rc;

private:
	void renderLine(float x1, float y1, float x2, float y2);
	void shiftRect(CRect& rc, int dx, int dy);
};

#endif // SELECTION_H

