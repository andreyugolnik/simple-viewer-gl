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
	bool GetRect(CRect<int>& rc);

private:
	bool m_visible;
	int m_imageWidth, m_imageHeight;
	CRect<int> m_rc;

private:
	void renderLine(int x1, int y1, int x2, int y2);
	void shiftRect(CRect<int>& rc, int dx, int dy);
	void clampPoint(int& x, int& y);
};

#endif // SELECTION_H

