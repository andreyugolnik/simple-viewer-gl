/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "infobar.h"
#include "main.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <GL/glut.h>

CInfoBar::CInfoBar() : m_visible(true), m_height(18), m_fntSize(12) {
	m_bg.reset(new CQuad(0, 0));
	m_ft.reset(new CFTString(m_fntSize));
//	m_ft.reset(new CFTString("arial.ttf", m_fntSize));
}

CInfoBar::~CInfoBar() {
}

void CInfoBar::Render() {
	if(m_visible == true) {
		int w	= glutGet(GLUT_WINDOW_WIDTH);
		int h	= glutGet(GLUT_WINDOW_HEIGHT);

		glColor3f(0, 0, 0.1f);
		m_bg->SetSpriteSize(w, m_height);
		m_bg->Render(0, h - m_height);

		glColor3f(1, 1, 0.5f);
		m_ft->Render(2, h - (m_height - m_fntSize));
	}
}

void CInfoBar::Update(const InfoBar* p) {
	std::stringstream title;

	if(p != 0) {
		std::string name	= p->path;
		size_t pos	= name.find_last_of('/');
		if(std::string::npos != pos) {
			name	= name.substr(pos + 1);
		}

		std::stringstream dim;
		dim << p->width << " x " << p->height << " x " << p->bpp << " bpp";

		if(p->scale != 1) {
			dim << " (" << (int)(100 * p->scale) << "%)";
		}

		int i = 0;
		float file_size	= p->file_size;
		while(file_size > 1024) {
			file_size	/= 1024;
			i++;
		}
		const char* s[]	= { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
		std::stringstream size;
		size << std::fixed << std::setprecision(i) << file_size << " " << (s[i]);

		std::stringstream formated;
		if(p->files_count > 1) {
			formated << (p->index + 1) << " out " << p->files_count << " | ";
		}
		formated  << name << " | " << dim.str() << " | " << size.str();
		if(p->sub_count > 1) {
			formated << " | " << (p->sub_image + 1) << " (" << p->sub_count << ")";
		}

		m_bottominfo	= formated.str();

		// set window title
		if(p->files_count > 1) {
			title << (p->index + 1) << " out " << p->files_count << " - " << name << " - " << TITLE;
		}
		else {
			title << name << " - " << TITLE;
		}
	}
	else {
		m_bottominfo	= "";
		title << TITLE;
	}

	m_ft->Update(m_bottominfo.c_str());

	glutSetWindowTitle(title.str().c_str());
}
