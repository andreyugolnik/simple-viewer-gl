/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quadimage.h"
#include <iostream>

CQuadImage::CQuadImage(int tw, int th, const unsigned char* data, int bpp) :
	CQuad(tw, th, data, bpp), m_col(0), m_row(0) {
}

CQuadImage::~CQuadImage() {
}
