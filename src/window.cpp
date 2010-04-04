/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "window.h"
#include "main.h"

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

extern std::auto_ptr<CWindow> g_window;

CWindow::CWindow() :
	m_winW(0), m_winH(0), m_scale(1), m_windowed(true), m_fitImage(false), m_cusorVisible(true),
	m_lastMouseX(-1), m_lastMouseY(-1), m_mouseLB(false), m_keyPressed(false), m_mouseDx(0), m_mouseDy(0),
	m_renderNa(false), m_textureSize(256), m_quadsCount(0) {

		m_il.reset(new CImageLoader());
		m_cb.reset(new CCheckerboard());
		m_ib.reset(new CInfoBar());
}

CWindow::~CWindow() {
	QuadsIc it = m_quads.begin(), itEnd = m_quads.end();
	for( ; it != itEnd; ++it) {
		glDeleteTextures(1, &(*it).tex);
	}
}

bool CWindow::Init(int argc, char *argv[], const char* path) {
	m_filesList.reset(new CFilesList(path));
	if(m_filesList->GetName() != 0) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);// | GLUT_DEPTH);
		m_loadingTime	= glutGet(GLUT_ELAPSED_TIME);

		glutCreateWindow(TITLE);

		glutReshapeFunc(callbackResize);
		glutDisplayFunc(callbackRender);
		glutTimerFunc(2000, callbackTimerCursor, 1);
		glutKeyboardFunc(callbackKeyboard);
		glutMouseFunc(callbackMouseButtons);
		glutSpecialFunc(callbackKeyboardSpecial);
		//glutEntryFunc();
		glutMotionFunc(callbackMouse);
		glutPassiveMotionFunc(callbackMouse);
		glutMouseWheelFunc(callbackMouseWheel);

		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_textureSize);
		std::cout << "Max texture size: " << m_textureSize << ", ";
		int size	= std::max(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
		m_textureSize	= std::min(m_textureSize, size);
		float power   = log((float)m_textureSize) / log(2.0f);
		m_textureSize	= (int)pow(2.0f, (int)(ceil(power)));
		std::cout << "set max to: " << m_textureSize << std::endl;

//		m_pow2	= glutExtensionSupported("GL_ARB_texture_non_power_of_two");
//		std::cout << "Non Power of Two extension " << (m_pow2 ? "available" : "not available") << std::endl;

		// setup progress each 10%
		imlib_context_set_progress_function(callbackProgressLoading);
		imlib_context_set_progress_granularity(10);

		m_cb->Init();

		loadImage(0);

		return true;
	}
	return false;
}

void CWindow::SetProp(Property prop) {
	switch(prop) {
	case PROP_INFOBAR:
		m_ib->Show(false);
		break;
	case PROP_CHECKERS:
		m_cb->Enable(false);
		break;
	case PROP_FITIMAGE:
		m_fitImage	= true;
		break;
	case PROP_FULLSCREEN:
		m_windowed	= false;
		break;
	}
}

void CWindow::SetProp(unsigned char r, unsigned char g, unsigned char b) {
	m_cb->SetColor(r, g, b);
}

void CWindow::fnRender() {
	m_cb->Render();

	glColor3f(1, 1, 1);

	if(m_renderNa == true) {
		m_cb->RenderNa();
	}
	else {
		calculateScale();

		if(m_mouseLB == true || m_keyPressed == true) {
			m_keyPressed	= false;
			int w	= (int)(m_il->GetWidth() * m_scale);
			int h	= (int)(m_il->GetHeight() * m_scale);

			const int delta	= 20;
			m_mouseDx	= std::max(m_mouseDx, delta - w);
			m_mouseDx	= std::min(m_mouseDx, m_winW - delta);
			m_mouseDy	= std::max(m_mouseDy, delta - h);
			m_mouseDy	= std::min(m_mouseDy, m_winH - delta);
		}

		for(int i = 0; i < m_quadsCount; i++) {
			QuadImg quad	= m_quads[i];

			float x	= m_mouseDx + quad.col * m_textureSize * m_scale;
			float y	= m_mouseDy + quad.row * m_textureSize * m_scale;

			float quadw	= quad.w * m_scale;
			float quadh	= quad.h * m_scale;
			quad.v[0].x = x;			quad.v[0].y = y;
			quad.v[1].x = x + quadw;	quad.v[1].y = y;
			quad.v[2].x = x + quadw;	quad.v[2].y = y + quadh;
			quad.v[3].x = x;			quad.v[3].y = y + quadh;

			if(x + quadw >= 0 && x < m_winW && y + quadh >= 0 && y < m_winH) {
				glBindTexture(GL_TEXTURE_2D, quad.tex);
				glBegin(GL_QUADS);
					glTexCoord2fv(&quad.v[0].tx);	glVertex2fv(&quad.v[0].x);
					glTexCoord2fv(&quad.v[1].tx);	glVertex2fv(&quad.v[1].x);
					glTexCoord2fv(&quad.v[2].tx);	glVertex2fv(&quad.v[2].x);
					glTexCoord2fv(&quad.v[3].tx);	glVertex2fv(&quad.v[3].x);
				glEnd();
			}
		}
	}

	m_ib->Render();

	glutSwapBuffers();
}

void CWindow::fnResize(int width, int height) {
	m_winW	= width;
	m_winH	= height;

//	if(m_winW < DEF_WINDOW_W || m_winH < DEF_WINDOW_H) {
//		m_winW	= std::max(m_winW, DEF_WINDOW_W);
//		m_winH	= std::max(m_winH, DEF_WINDOW_H);
//		glutReshapeWindow(m_winW, m_winH);
//	}

	calculateScale();
	int w	= (int)(m_il->GetWidth() * m_scale);
	int h	= (int)(m_il->GetHeight() * m_scale);
	m_mouseDx	= (m_winW - w) / 2;
	m_mouseDy	= (m_winH - h) / 2;

	glViewport(0, 0, m_winW, m_winH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_winW, m_winH, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CWindow::fnMouse(int x, int y) {
	showCursor(true);

	int diffx	= x - m_lastMouseX;
	int diffy	= y - m_lastMouseY;
	m_lastMouseX	= x;
	m_lastMouseY	= y;
	if(m_fitImage == false && m_mouseLB == true) {
		if(diffx != 0 || diffy != 0) {
			m_mouseDx	+= diffx;
			m_mouseDy	+= diffy;
			glutPostRedisplay();
		}
	}
}

void CWindow::fnMouseWheel(int wheel, int direction, int x, int y) {
	if(direction > 0) {
		fnKeyboard('+', 0, 0);
	}
	else {//if(direction < 0) {
		fnKeyboard('-', 0, 0);
	}
}

void CWindow::fnMouseButtons(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON) {
		m_mouseLB	= (state == GLUT_DOWN);
	}
}

void CWindow::fnKeyboard(unsigned char key, int x, int y) {
//	GLUT_ACTIVE_SHIFT
//	GLUT_ACTIVE_CTRL
//	GLUT_ACTIVE_ALT
	int mod	= glutGetModifiers();

	switch(key) {
	case 27:	// ESC
		exit(0);
		break;
	case 127:	// Delete
		if(mod == GLUT_ACTIVE_CTRL) {
			m_filesList->RemoveFromDisk();
		}
		break;
	case 'i':
	case 'I':
		m_ib->Show(!m_ib->Visible());
		glutPostRedisplay();
		break;
	case 's':
	case 'S':
		m_fitImage	= !m_fitImage;
		if(m_fitImage == false) {
			m_scale	= 1;
		}
		fnResize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		updateInfobar();
		glutPostRedisplay();
		break;
	case ' ':
		loadImage(1);
		glutPostRedisplay();
		break;
	case 8:	// backspace
		loadImage(-1);
		glutPostRedisplay();
		break;
	case '+':
	case '=':
		m_scale	/= 0.95f;
		m_fitImage	= false;
		updateInfobar();
		glutPostRedisplay();
		break;
	case '-':
		m_scale	*= 0.95f;
		m_fitImage	= false;
		updateInfobar();
		glutPostRedisplay();
		break;
	case 'c':
	case 'C':
		m_cb->Enable(!m_cb->IsEnabled());
		glutPostRedisplay();
		break;
	case '0':
		m_scale	= 1;
		m_fitImage	= false;
		fnResize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		updateInfobar();
		glutPostRedisplay();
		break;
	case 13:
		m_windowed	= !m_windowed;
		if(m_windowed	== false) {
			glutFullScreen();
		}
		else {
			glutReshapeWindow(m_winW, m_winH);
		}
		glutPostRedisplay();
		break;
//	default:
//		std::cout << key << std::endl;
//		break;
	}
}

void CWindow::fnKeyboardSpecial(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_LEFT:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_mouseDx	+= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_RIGHT:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_mouseDx	-= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_UP:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_mouseDy	+= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_DOWN:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_mouseDy	-= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_PAGE_UP:
		break;
	case GLUT_KEY_PAGE_DOWN:
		break;
	}
}




void CWindow::calculateScale() {
	if(m_fitImage == true) {
		int w	= m_il->GetWidth();
		int h	= m_il->GetHeight();

        // scale only large images
        if(w >= m_winW || h >= m_winH) {
            float aspect  = (float)w / h;
            float nW = 0;
            float nH = 0;
            float dx	= (float)w / m_winW;
            float dy	= (float)h / m_winH;
            if(dx > dy) {
                if(w > m_winW) {
                    nW  = m_winW;
                    nH  = nW / aspect;
                }
            }
            else {
                if(h > m_winH) {
                    nH  = m_winH;
                    nW  = nH * aspect;
                }
            }
            if(nW != 0 && nH != 0) {
//				m_scale    = (float)((g_nAngle == 0 || g_nAngle == 180) ? nW : nH) / w;
                m_scale    = nW / w;
            }
        }
        else {
        	m_scale    = 1;
        }
	}
}

//void CWindow::centerWindow() {
//	if(m_windowed == true) {
//		calculateScale();
//		int w	= m_il->GetWidth() * m_scale;
//		int h	= m_il->GetHeight() * m_scale;
//		int scrw	= glutGet(GLUT_SCREEN_WIDTH);
//		int scrh	= glutGet(GLUT_SCREEN_HEIGHT);
//		int winw	= std::min(w != 0 ? w : DEF_WINDOW_W, scrw);
//		int winh	= std::min(h != 0 ? h : DEF_WINDOW_H, scrh);
//		glutReshapeWindow(winw, winh);
//		int posx	= (scrw - winw) / 2;
//		int posy	= (scrh - winh) / 2;
//		glutPositionWindow(posx, posy);
//		printf("%d x %d | %d x %d | %d, %d\n", scrw, scrh, winw, winh, posx, posy);
//	}
//}
//
bool CWindow::loadImage(int step) {
	bool ret	= false;

	if(step != 0) {
		m_filesList->ParseDir();
		m_il->SetAngle(ANGLE_0);
		m_scale		= 1;
		m_quadsCount	= 0;
		m_renderNa	= false;
	}

	const char* path	= m_filesList->GetName(step);
	if(path != 0) {
		// loading progress by callback
		std::cout << "Loading";
		m_loadingTime	= glutGet(GLUT_ELAPSED_TIME);

		if(true == m_il->LoadImage(path, 0)) {
			unsigned char* bitmap	= m_il->GetBitmap();
			createTextures(m_il->GetWidth(), m_il->GetHeight(), m_il->HasAlpha(), bitmap);

			ret	= true;
		}
		else {
			m_renderNa	= true;
		}

		updateInfobar();

		fnResize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//		centerWindow();
	}

	return ret;
}

void CWindow::updateInfobar() {
	INFO_BAR s;
	s.path			= m_filesList->GetName(0);
	s.index			= m_filesList->GetIndex();
	s.width			= m_il->GetWidth();
	s.height		= m_il->GetHeight();
	s.bpp			= m_il->GetBpp();
	s.scale			= m_scale;
	s.sub_image		= m_il->GetSub();
	s.sub_count		= m_il->GetSubCount();
	s.file_size		= m_il->GetSize();
	s.files_count	= m_filesList->GetCount();
	m_ib->Update(&s);
}

void CWindow::createTextures(int width, int height, bool alpha, unsigned char* bitmap) {
	std::cout << " " << width << " x " << height << ", ";

	int cols	= (int)ceilf((float)width / m_textureSize);
	int rows	= (int)ceilf((float)height / m_textureSize);
	m_quadsCount	= cols * rows;
	std::cout << "textures: " << m_quadsCount << " (" << cols << " x " << rows << ")" << std::endl;

	while(m_quadsCount > (int)m_quads.size()) {
		QuadImg quad;
		glGenTextures(1, &quad.tex);
		glBindTexture(GL_TEXTURE_2D, quad.tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		m_quads.push_back(quad);
	}

	GLenum fmt	= (alpha == true ? GL_BGRA : GL_BGR);

	unsigned char* buffer	= new unsigned char[m_textureSize * m_textureSize * (alpha == true ? 4 : 3)];

	int idx	= 0;
	int height2	= height;
	for(int row = 0; row < rows; row++) {
		int width2	= width;
		int h	= (height2 > m_textureSize ? m_textureSize : height2);
		for(int col = 0; col < cols; col++) {
			glBindTexture(GL_TEXTURE_2D, m_quads[idx].tex);

			int w	= (width2 > m_textureSize ? m_textureSize : width2);
			width2	-= w;

			copyBuffer(bitmap, col, row, width, alpha, buffer, w, h);

			glTexImage2D(GL_TEXTURE_2D, 0, (alpha == true ? 4 : 3), m_textureSize, m_textureSize, 0, fmt, GL_UNSIGNED_BYTE, buffer);
			int e	= glGetError();
			if(GL_NO_ERROR != e) {
				std::cout << "can't update texture " << idx << ": " << e << std::endl;
//				const GLubyte* s   = gluErrorString(e);
			}

			m_quads[idx].col	= col;
			m_quads[idx].row	= row;
			m_quads[idx].w	= w;
			m_quads[idx].h	= h;

			m_quads[idx].v[0].tx = 0;							m_quads[idx].v[0].ty = 0;
			m_quads[idx].v[1].tx = w / (float)m_textureSize;	m_quads[idx].v[1].ty = 0;
			m_quads[idx].v[2].tx = w / (float)m_textureSize;	m_quads[idx].v[2].ty = h / (float)m_textureSize;
			m_quads[idx].v[3].tx = 0;							m_quads[idx].v[3].ty = h / (float)m_textureSize;

			idx++;
		}
		height2	-= h;
	}

	delete[] buffer;
}


void CWindow::copyBuffer(unsigned char* bitmap, int col, int row, int width, bool alpha, unsigned char* buffer, int w, int h) {
	int bitmap_pitch	= width * 4;
	int dx	= col * m_textureSize * 4;
	int dy	= row * m_textureSize;
	int txt_pitch	= m_textureSize * (alpha == true ? 4 : 3);
	int count	= w * (alpha == true ? 4 : 3);
	for(int line = 0; line < h; line++) {
		int src	= dx + (dy + line) * bitmap_pitch;
		int dst	= line * txt_pitch;
		if(alpha == true) {
			memcpy(&buffer[dst], &bitmap[src], count);
		}
		else {
			unsigned char* psrc	= &bitmap[src];
			unsigned char* pdst	= &buffer[dst];
			for(int x = 0; x < w; x++) {
				*pdst++	= *psrc++;
				*pdst++	= *psrc++;
				*pdst++	= *psrc++;
				psrc++;
			}
		}
	}
}

void CWindow::showCursor(bool show) {
	if(m_cusorVisible != show) {
		m_cusorVisible	= show;
		glutSetCursor(show == true ? GLUT_CURSOR_RIGHT_ARROW : GLUT_CURSOR_NONE);
		if(m_cusorVisible == true) {
			glutTimerFunc(2000, callbackTimerCursor, 1);
		}
	}
}




void CWindow::callbackResize(int width, int height) {
	g_window->updateInfobar();
	g_window->fnResize(width, height);
}

void CWindow::callbackRender() {
	g_window->fnRender();
}

void CWindow::callbackTimerCursor(int value) {
	g_window->showCursor(false);
}

void CWindow::callbackMouse(int x, int y) {
	g_window->fnMouse(x, y);
}

void CWindow::callbackMouseButtons(int button, int state, int x, int y) {
	g_window->fnMouseButtons(button, state, x, y);
}

void CWindow::callbackMouseWheel(int wheel, int direction, int x, int y) {
	g_window->fnMouseWheel(wheel, direction, x, y);
}

void CWindow::callbackKeyboardSpecial(int key, int x, int y) {
	g_window->fnKeyboardSpecial(key, x, y);
}

void CWindow::callbackKeyboard(unsigned char key, int x, int y) {
	g_window->fnKeyboard(key, x, y);
}


void CWindow::fnProgressLoading(Imlib_Image im, char percent, int update_x, int update_y, int update_w, int update_h) {
	std::cout << ".";
	if(m_loadingTime + 1000 < glutGet(GLUT_ELAPSED_TIME)) {
		m_cb->RenderLoading();
		glutSwapBuffers();
	}
}

int CWindow::callbackProgressLoading(Imlib_Image im, char percent, int update_x, int update_y, int update_w, int update_h) {
	g_window->fnProgressLoading(im, percent, update_x, update_y, update_w, update_h);
	return 1;
}

