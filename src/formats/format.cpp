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
	m_bpp(0), m_bppImage(0),
	m_size(-1),	// -1 mean that file can't be opened
	m_sizeMem(0),
	m_subImage(0),
	m_subCount(0)
{
}

CFormat::~CFormat() {
}

bool CFormat::openFile(const char* path) {
	m_file	= fopen(path, "rb");
	if(m_file == 0) {
		std::cout << "Can't open \"" << path << "\"." << std::endl;
		return false;
	}

	fseek(m_file, 0, SEEK_END);
	m_size	= ftell(m_file);
	fseek(m_file, 0, SEEK_SET);

	return true;
}

void CFormat::convertBGR2RGB() {
	int bytes	= m_bpp / 8;
	int size	= m_width * m_height * bytes;

	for(int i = 0; i < size; i += bytes) {
		unsigned char b	= m_bitmap[i];
		m_bitmap[i]		= m_bitmap[i + 2];
		m_bitmap[i + 2]	= b;
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

uint16 CFormat::read_uint16(uint8* p) {
    return (p[0] << 8) | p[1];
}

uint32 CFormat::read_uint32(uint8* p) {
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}
