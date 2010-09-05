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
#include "quad.h"

#include <memory>

class CSelection {
public:
	CSelection();
	virtual ~CSelection();

	void Init();
	void SetImageDimension(int w, int h);
	void StartPoint(int x, int y);
	void EndPoint(int x, int y);
	void Render(int dx, int dy);
	CRect<int> GetRect() const;

private:
	bool m_enabled;
	int m_imageWidth, m_imageHeight;
	std::auto_ptr<CQuad> m_selection;
	CRect<int> m_rc;

private:
	void renderLine(int x1, int y1, int x2, int y2);
	void setImagePos(CRect<int>& rc, int dx, int dy);
	void clampPoint(int& x, int& y);
};

#endif // SELECTION_H

