/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quad.h"
#include <iostream>

CQuad::CQuad(int tw, int th, const unsigned char* data, int bpp) : m_tex(0), m_w(0), m_h(0) {
	init(tw, th, data, bpp);
}

CQuad::~CQuad() {
	if(m_tex != 0) {
		glDeleteTextures(1, &m_tex);
	}
}

void CQuad::init(int tw, int th, const unsigned char* data, int bpp) {
	if(m_tex == 0) {
		m_tw	= tw;
		m_th	= th;

		// by deafult set whole texture size
		SetSpriteSize(tw, th);

		if(data != 0) {
			glGenTextures(1, &m_tex);

			// TODO move out of this method
			glBindTexture(GL_TEXTURE_2D, m_tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0, bpp, tw, th, 0, bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
			int e	= glGetError();
			if(GL_NO_ERROR != e) {
		//		const GLubyte* s   = gluErrorString(e);
				std::cout << "can't update texture " << m_tex << ": " << e << std::endl;
			}
		}
	}
}

void CQuad::SetSpriteSize(float w, float h) {
	m_w	= w;
	m_h	= h;

	m_v[0].tx = 0;			m_v[0].ty = 0;
	m_v[1].tx = w / m_tw;	m_v[1].ty = 0;
	m_v[2].tx = w / m_tw;	m_v[2].ty = h / m_th;
	m_v[3].tx = 0;			m_v[3].ty = h / m_th;
}

void CQuad::Render(float x, float y) {
	m_v[0].x = x;		m_v[0].y = y;
	m_v[1].x = x + m_w;	m_v[1].y = y;
	m_v[2].x = x + m_w;	m_v[2].y = y + m_h;
	m_v[3].x = x;		m_v[3].y = y + m_h;

	glBindTexture(GL_TEXTURE_2D, m_tex);
	glBegin(GL_QUADS);
		glTexCoord2fv(&m_v[0].tx);	glVertex2fv(&m_v[0].x);
		glTexCoord2fv(&m_v[1].tx);	glVertex2fv(&m_v[1].x);
		glTexCoord2fv(&m_v[2].tx);	glVertex2fv(&m_v[2].x);
		glTexCoord2fv(&m_v[3].tx);	glVertex2fv(&m_v[3].x);
	glEnd();
}

void CQuad::RenderEx(float x, float y, float w, float h) {
	m_v[0].x = x;		m_v[0].y = y;
	m_v[1].x = x + w;	m_v[1].y = y;
	m_v[2].x = x + w;	m_v[2].y = y + h;
	m_v[3].x = x;		m_v[3].y = y + h;

	glBindTexture(GL_TEXTURE_2D, m_tex);
	glBegin(GL_QUADS);
		glTexCoord2fv(&m_v[0].tx);	glVertex2fv(&m_v[0].x);
		glTexCoord2fv(&m_v[1].tx);	glVertex2fv(&m_v[1].x);
		glTexCoord2fv(&m_v[2].tx);	glVertex2fv(&m_v[2].x);
		glTexCoord2fv(&m_v[3].tx);	glVertex2fv(&m_v[3].x);
	glEnd();
}
