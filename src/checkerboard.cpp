/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "checkerboard.h"
#include "img-loading.c"
#include "img-na.c"
#include <math.h>

const int tex_size	= 128;

CCheckerboard::CCheckerboard() : m_enabled(false), m_r(0), m_g(0), m_b(1) {
}
CCheckerboard::~CCheckerboard() {
	glDeleteTextures(1, &m_cb.tex);
	glDeleteTextures(1, &m_loading.tex);
	glDeleteTextures(1, &m_na.tex);
}

void CCheckerboard::Init() {
	// Create checkerboard texture
	//
	glGenTextures(1, &m_cb.tex);
	glBindTexture(GL_TEXTURE_2D, m_cb.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	unsigned char* buffer	= new unsigned char[tex_size * tex_size * 3];

	unsigned char* p	= buffer;
	bool checker_height_odd	= true;
	for(int y = 0; y < tex_size; y++) {
		if(y % 16 == 0) {
			checker_height_odd	= !checker_height_odd;
		}

		bool checker_width_odd	= checker_height_odd;
		for(int x = 0; x < tex_size; x++) {
			if(x % 16 == 0) {
				checker_width_odd	= !checker_width_odd;
			}

			unsigned char color	= (checker_width_odd == true ? 0xc8 : 0x7d);
			*p++	= color;
			*p++	= color;
			*p++	= color;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_size, tex_size, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);

	delete[] buffer;


	// Create loading texture
	//
	glGenTextures(1, &m_loading.tex);
	glBindTexture(GL_TEXTURE_2D, m_loading.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	m_loading.v[0].tx = 0;	m_loading.v[0].ty = 0;
	m_loading.v[1].tx = 1;	m_loading.v[1].ty = 0;
	m_loading.v[2].tx = 1;	m_loading.v[2].ty = 1;
	m_loading.v[3].tx = 0;	m_loading.v[3].ty = 1;

	glTexImage2D(GL_TEXTURE_2D, 0, imgLoading.bytes_per_pixel, imgLoading.width, imgLoading.height, 0,
		imgLoading.bytes_per_pixel == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imgLoading.pixel_data);


	// Create N/A texture
	//
	glGenTextures(1, &m_na.tex);
	glBindTexture(GL_TEXTURE_2D, m_na.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	m_na.v[0].tx = 0;	m_na.v[0].ty = 0;
	m_na.v[1].tx = 1;	m_na.v[1].ty = 0;
	m_na.v[2].tx = 1;	m_na.v[2].ty = 1;
	m_na.v[3].tx = 0;	m_na.v[3].ty = 1;

	glTexImage2D(GL_TEXTURE_2D, 0, imgNa.bytes_per_pixel, imgNa.width, imgNa.height, 0,
		imgNa.bytes_per_pixel == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imgNa.pixel_data);
}

void CCheckerboard::SetColor(unsigned char r, unsigned char g, unsigned char b) {
	m_r	= r / 255.0f;
	m_g	= g / 255.0f;
	m_b	= b / 255.0f;
}

void CCheckerboard::Render() {
	glEnable(GL_TEXTURE_2D);

	if(m_enabled == true) {
		glColor3f(1, 1, 1);

		float w	= (float)glutGet(GLUT_WINDOW_WIDTH);
		float h	= (float)glutGet(GLUT_WINDOW_HEIGHT);
		m_cb.v[0].x = 0;	m_cb.v[0].y = 0;
		m_cb.v[1].x = w;	m_cb.v[1].y = 0;
		m_cb.v[2].x = w;	m_cb.v[2].y = h;
		m_cb.v[3].x = 0;	m_cb.v[3].y = h;

		m_cb.v[0].tx = 0;			m_cb.v[0].ty = 0;
		m_cb.v[1].tx = w/tex_size;	m_cb.v[1].ty = 0;
		m_cb.v[2].tx = w/tex_size;	m_cb.v[2].ty = h/tex_size;
		m_cb.v[3].tx = 0;			m_cb.v[3].ty = h/tex_size;

		glBindTexture(GL_TEXTURE_2D, m_cb.tex);
		glBegin(GL_QUADS);
			glTexCoord2fv(&m_cb.v[0].tx);	glVertex2fv(&m_cb.v[0].x);
			glTexCoord2fv(&m_cb.v[1].tx);	glVertex2fv(&m_cb.v[1].x);
			glTexCoord2fv(&m_cb.v[2].tx);	glVertex2fv(&m_cb.v[2].x);
			glTexCoord2fv(&m_cb.v[3].tx);	glVertex2fv(&m_cb.v[3].x);
		glEnd();

	}
	else {
		glClearColor(m_r, m_g, m_b, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void CCheckerboard::RenderLoading() {
	glEnable(GL_TEXTURE_2D);

	glColor3f(1, 1, 1);

	float w	= (float)glutGet(GLUT_WINDOW_WIDTH);
	float h	= (float)glutGet(GLUT_WINDOW_HEIGHT);
	float x	= ceil((w - imgLoading.width) / 2);
	float y	= ceil((h - imgLoading.height) / 2);

	m_loading.v[0].x = x;						m_loading.v[0].y = y;
	m_loading.v[1].x = x + imgLoading.width;	m_loading.v[1].y = y;
	m_loading.v[2].x = x + imgLoading.width;	m_loading.v[2].y = y + imgLoading.height;
	m_loading.v[3].x = x;						m_loading.v[3].y = y + imgLoading.height;

	glBindTexture(GL_TEXTURE_2D, m_loading.tex);
	glBegin(GL_QUADS);
		glTexCoord2fv(&m_loading.v[0].tx);	glVertex2fv(&m_loading.v[0].x);
		glTexCoord2fv(&m_loading.v[1].tx);	glVertex2fv(&m_loading.v[1].x);
		glTexCoord2fv(&m_loading.v[2].tx);	glVertex2fv(&m_loading.v[2].x);
		glTexCoord2fv(&m_loading.v[3].tx);	glVertex2fv(&m_loading.v[3].x);
	glEnd();
}

void CCheckerboard::RenderNa() {
	glEnable(GL_TEXTURE_2D);

	glColor3f(1, 1, 1);

	float w	= (float)glutGet(GLUT_WINDOW_WIDTH);
	float h	= (float)glutGet(GLUT_WINDOW_HEIGHT);
	float x	= ceil((w - imgNa.width) / 2);
	float y	= ceil((h - imgNa.height) / 2);

	m_na.v[0].x = x;			m_na.v[0].y = y;
	m_na.v[1].x = x + imgNa.width;	m_na.v[1].y = y;
	m_na.v[2].x = x + imgNa.width;	m_na.v[2].y = y + imgNa.height;
	m_na.v[3].x = x;			m_na.v[3].y = y + imgNa.height;

	glBindTexture(GL_TEXTURE_2D, m_na.tex);
	glBegin(GL_QUADS);
		glTexCoord2fv(&m_na.v[0].tx);	glVertex2fv(&m_na.v[0].x);
		glTexCoord2fv(&m_na.v[1].tx);	glVertex2fv(&m_na.v[1].x);
		glTexCoord2fv(&m_na.v[2].tx);	glVertex2fv(&m_na.v[2].x);
		glTexCoord2fv(&m_na.v[3].tx);	glVertex2fv(&m_na.v[3].x);
	glEnd();
}
