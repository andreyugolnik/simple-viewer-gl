/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUADIMAGE_H
#define QUADIMAGE_H

#include "quad.h"

class CQuadImage : public CQuad
{
public:
	CQuadImage(int tw, int th, const unsigned char* data, GLenum bitmapFormat);
	virtual ~CQuadImage();

	void SetCell(int col, int row) { m_col = col; m_row = row; }
	int GetCol() const { return m_col; }
	int GetRow() const { return m_row; }

private:
	int m_col;
	int m_row;

private:
	CQuadImage();
};

#endif // QUADIMAGE_H

