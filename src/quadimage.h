/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUADIMAGE_H
#define QUADIMAGE_H

#include "quad.h"

class CQuadImage : public CQuad {
public:
	CQuadImage(int tex_size);
	virtual ~CQuadImage();

	void SetCell(int col, int row) { m_col = col; m_row = row; }
	int GetCol() const { return m_col; }
	int GetRow() const { return m_row; }
	void Update(int bpp, const unsigned char* buffer);

private:
	int m_tex_size;
	int m_col, m_row;

private:
	CQuadImage();
};

#endif // QUADIMAGE_H
