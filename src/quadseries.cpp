/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quadseries.h"
#include <iostream>

CQuadSeries::CQuadSeries(int tw, int th, const unsigned char* data, GLenum bitmapFormat) :
	CQuad(tw, th, data, bitmapFormat), m_count(1), m_frame(0) {
}

CQuadSeries::~CQuadSeries() {
}

void CQuadSeries::Setup(int width, int height, int count) {
	m_w		= width;
	m_h		= height;
	m_count	= count;
}

void CQuadSeries::SetFrame(int frame) {
	m_frame	= frame;
	m_frame	%= m_count;

	int cols	= (int)(m_tw / m_w);

	float tx1	= m_frame * m_w;
	float ty1	= 0;

	if(tx1 > m_tw - m_w && cols > 0) {
		m_frame	-= (int)(m_tw / m_w);
		tx1		=  m_w * (m_frame % cols);
		ty1		+= m_h * (1 + m_frame / cols);
	}

	float tx2	= (tx1 + m_w) / m_tw;
	float ty2	= (ty1 + m_h) / m_th;

	tx1	/= m_tw;
	ty1	/= m_th;

	m_v[0].tx = tx1;	m_v[0].ty = ty1;
	m_v[1].tx = tx2;	m_v[1].ty = ty1;
	m_v[2].tx = tx2;	m_v[2].ty = ty2;
	m_v[3].tx = tx1;	m_v[3].ty = ty2;
}