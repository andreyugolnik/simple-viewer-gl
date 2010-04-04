/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "progress.h"
#include "img-loading.c"
#include <iostream>
#include <math.h>

CProgress::CProgress() : m_loadingTime(0), m_progress(0) {
}

CProgress::~CProgress() {
}

void CProgress::Init() {
	m_loading.reset(new CQuad(imgLoading.width, imgLoading.height, imgLoading.pixel_data, imgLoading.bytes_per_pixel));
	m_square.reset(new CQuad(4, 4));
}

void CProgress::Start() {
	std::cout << "Loading";
	m_loadingTime	= glutGet(GLUT_ELAPSED_TIME);
	m_progress		= 0;
}

void CProgress::Render() {
	m_progress++;

	if(m_loadingTime + 1000 < glutGet(GLUT_ELAPSED_TIME)) {
		std::cout << ".";

		float w	= (float)glutGet(GLUT_WINDOW_WIDTH);
		float h	= (float)glutGet(GLUT_WINDOW_HEIGHT);
		float x	= ceil((w - imgLoading.width) / 2);
		float y	= ceil((h - imgLoading.height) / 2);

		glColor3f(1, 1, 1);
		m_loading->Render(x, y);

		// render progress
		for(int i = 0; i < m_progress; i++) {
			m_square->Render(i * 6, 6);
		}

		glutSwapBuffers();
	}
}
