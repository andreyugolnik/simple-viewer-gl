/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quadimage.h"
#include <iostream>

CQuadImage::CQuadImage(int tex_size) :
	CQuad(tex_size, tex_size), m_tex_size(tex_size), m_col(0), m_row(0) {

	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

CQuadImage::~CQuadImage() {
}

void CQuadImage::Update(int bpp, const unsigned char* buffer) {
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glTexImage2D(GL_TEXTURE_2D, 0, bpp, m_tex_size, m_tex_size, 0, bpp == 4 ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, buffer);
	int e	= glGetError();
	if(GL_NO_ERROR != e) {
//		const GLubyte* s   = gluErrorString(e);
		std::cout << "can't update texture " << m_tex << ": " << e << std::endl;
	}
}
