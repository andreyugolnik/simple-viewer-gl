/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "infobar.h"
#include "main.h"
#include <iomanip>
#include <sstream>
#include <GL/glut.h>

CInfoBar::CInfoBar() : m_visible(true) {
}

CInfoBar::~CInfoBar() {
}

void CInfoBar::Render() {
	if(m_visible == true) {
		const int x	= 5;
		int height	= glutGet(GLUT_WINDOW_HEIGHT) - 5;

		glDisable(GL_TEXTURE_2D);
		glColor3f(0, 0, 0);
		glRasterPos2i(x+1, height+1);

	//	glcRenderString(string);
		const char* p	= m_bottominfo.c_str();
		for( ; *p; p++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (unsigned char)*p);
			//glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
		}

		glColor3f(0, 1, 0);
		glRasterPos2i(x, height);

		p	= m_bottominfo.c_str();
		for( ; *p; p++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (unsigned char)*p);
		}
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
		size << std::setprecision(3) << file_size << " " << (s[i]);

		std::stringstream formated;
		if(p->files_count > 1) {
			formated << (p->index + 1) << " out " << p->files_count << ", ";
		}
		formated  << name << " | " << dim.str() << " | " << size.str();
		if(p->sub_count > 1) {
			formated << " | " << (p->sub_image + 1) << " (" << p->sub_count << ")";
		}

		m_bottominfo	= formated.str();

		// set window title
		if(p->files_count > 1) {
			title << (p->index + 1) << " out " << p->files_count << ", " << name << " - " << TITLE;
		}
		else {
			title << name << " - " << TITLE;
		}
	}
	else {
		m_bottominfo	= "";
		title << TITLE;
	}

	glutSetWindowTitle(title.str().c_str());
}
//
//void CInfoBar::Update(const char* text) {
//	std::stringstream title;
//
//	if(text != 0) {
//		m_bottominfo	= text;
//		title << text << " - " << TITLE;;
//	}
//	else {
//		m_bottominfo	= "";
//		title << TITLE;;
//	}
//
//	glutSetWindowTitle(title.str().c_str());
//}
