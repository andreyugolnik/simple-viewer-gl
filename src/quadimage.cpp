/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quadimage.h"

CQuadImage::CQuadImage(int tw, int th, const unsigned char* data, GLenum bitmapFormat) :
	CQuad(tw, th, data, bitmapFormat), m_col(0), m_row(0) {
}

CQuadImage::~CQuadImage() {
}
