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

CProgress::CProgress() : m_loadingTime(0) {
}

CProgress::~CProgress() {
}

void CProgress::Init() {
	m_loading.reset(new CQuad(imgLoading.width, imgLoading.height, imgLoading.pixel_data, imgLoading.bytes_per_pixel * 8));
	m_square.reset(new CQuad(4, 4));
}

void CProgress::Start() {
	std::cout << "Loading...";
	m_loadingTime	= glutGet(GLUT_ELAPSED_TIME);
}

void CProgress::Render(int percent) {
	if(m_loadingTime + 1000 < glutGet(GLUT_ELAPSED_TIME)) {
		float w	= (float)glutGet(GLUT_WINDOW_WIDTH);
		float h	= (float)glutGet(GLUT_WINDOW_HEIGHT);
		float x	= ceil((w - imgLoading.width) / 2);
		float y	= ceil((h - imgLoading.height) / 2);

		glColor3f(1, 1, 1);
		m_loading->Render(x, y);

		const int count		= 20;
		const float step	= imgLoading.width / count;
		x	= ceil((w - step * count) / 2);
		for(int i = 0; i < percent / (100 / count); i++) {
			m_square->Render(x + i * step, y + imgLoading.height);
		}

		glutSwapBuffers();
	}
}
