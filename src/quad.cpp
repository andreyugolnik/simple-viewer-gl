/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quad.h"

#include <iostream>
#include <math.h>

CQuad::CQuad(int tw, int th, const unsigned char* data, GLenum bitmapFormat) :
	m_tw(tw), m_th(th), m_tex(0), m_w(0) {

	if(data != 0) {
		glGenTextures(1, &m_tex);

		glBindTexture(GL_TEXTURE_2D, m_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

//		std::cout << "creating " << tw << " x " << th << " texture" << std::endl;
		GLint bytes	= ((bitmapFormat == GL_RGBA || bitmapFormat == GL_BGRA) ? 4 : 3);
		glTexImage2D(GL_TEXTURE_2D, 0, bytes, tw, th, 0, bitmapFormat, GL_UNSIGNED_BYTE, data);
		int e	= glGetError();
		if(GL_NO_ERROR != e) {
	//		const GLubyte* s   = gluErrorString(e);
			std::cout << "can't update texture " << m_tex << ": " << e << std::endl;
		}
	}

	// by deafult set whole texture size
	SetSpriteSize(tw, th);
}

CQuad::~CQuad() {
	if(m_tex != 0) {
		glDeleteTextures(1, &m_tex);
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

void CQuad::SetWindowSize(float w, float h) {
	m_rcWindow.Set(0, 0, w, h);
}

void CQuad::Render(float x, float y) {
	RenderEx(x, y, m_w, m_h);
}

void CQuad::RenderEx(float x, float y, float w, float h, int angle) {
    if(angle != 0) {
		float a	= M_PI * angle / 180;
		float c  = cosf(a);
		float s  = sinf(a);

		m_v[0].x  = x;				m_v[0].y  = y;
		m_v[1].x  = x + w*c;		m_v[1].y  = y + w*s;
		m_v[2].x  = x + w*c - h*s;	m_v[2].y  = y + w*s + h*c;
		m_v[3].x  = x - h*s;		m_v[3].y  = y + h*c;
    }
    else {
		m_v[0].x = x;		m_v[0].y = y;
		m_v[1].x = x + w;	m_v[1].y = y;
		m_v[2].x = x + w;	m_v[2].y = y + h;
		m_v[3].x = x;		m_v[3].y = y + h;
    }

	CRect<float> rc;
	rc.Encapsulate(m_v[0].x, m_v[0].y);
	rc.Encapsulate(m_v[1].x, m_v[1].y);
	rc.Encapsulate(m_v[2].x, m_v[2].y);
	rc.Encapsulate(m_v[3].x, m_v[3].y);

	if(rc.IsSet() == false || m_rcWindow.IsSet() == false || rc.Intersect(&m_rcWindow) == true) {
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glBegin(GL_QUADS);
			glTexCoord2fv(&m_v[0].tx);	glVertex2fv(&m_v[0].x);
			glTexCoord2fv(&m_v[1].tx);	glVertex2fv(&m_v[1].x);
			glTexCoord2fv(&m_v[2].tx);	glVertex2fv(&m_v[2].x);
			glTexCoord2fv(&m_v[3].tx);	glVertex2fv(&m_v[3].x);
		glEnd();
	}
}
