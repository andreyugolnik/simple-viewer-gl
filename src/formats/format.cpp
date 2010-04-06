/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "format.h"

CFormat::CFormat(Callback callback) : m_callback(callback),
	m_percent(-1), m_file(0),
	m_bitmap(0), m_width(0), m_height(0), m_pitch(0),
	m_bpp(0), m_bppImage(0), m_size(0) {
}

CFormat::~CFormat() {
}

bool CFormat::openFile(const char* path) {
	m_file	= fopen(path, "rb");
	if(m_file == 0) {
		printf("Can't open %s\n", path);
		return false;
	}

	fseek(m_file, 0, SEEK_END);
	m_size	= ftell(m_file);
	fseek(m_file, 0, SEEK_SET);

	return true;
}

void CFormat::convertRGB2BGR() {
	int bytes	= m_bpp / 8;
	int size	= m_width * m_height * bytes;

	for(int i = 0; i < size; i += bytes) {
		unsigned char r	= m_bitmap[i];
		m_bitmap[i]		= m_bitmap[i + 2];
		m_bitmap[i + 2]	= r;
	}
}

void CFormat::progress(int percent) {
	if(m_callback != 0) {
		if(m_percent != percent) {
			m_percent	= percent;
			m_callback(percent);
		}
	}
}
