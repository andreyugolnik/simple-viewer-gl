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

//void closeWindow() {
//	printf("done\n");
//}

CWindow::CWindow() : m_initialImageLoading(true),
	m_prevWinX(0), m_prevWinY(0), m_prevWinW(DEF_WINDOW_W), m_prevWinH(DEF_WINDOW_H),
	m_curWinW(0), m_curWinH(0), m_scale(1), m_windowed(true), m_fitImage(false), m_showBorder(false), m_recursiveDir(false), m_cusorVisible(true),
	m_lastMouseX(-1), m_lastMouseY(-1), m_mouseLB(false), m_keyPressed(false), m_imageDx(0), m_imageDy(0),
	m_pow2(false), m_textureSize(256) {

		m_il.reset(new CImageLoader(callbackProgressLoading));
		m_cb.reset(new CCheckerboard());
		m_na.reset(new CNotAvailable());
		m_ib.reset(new CInfoBar());
		m_progress.reset(new CProgress());
		m_border.reset(new CImageBorder());
}

CWindow::~CWindow() {
	deleteTextures();
}

bool CWindow::Init(int argc, char *argv[], const char* path) {
	m_filesList.reset(new CFilesList(path, m_recursiveDir));
	if(m_filesList->GetName() != 0) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

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
//		glutWMCloseFunc(closeWindow);

		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//		int version	= glutGet(GLUT_VERSION);
//		std::cout << "GLUT v" << version << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_textureSize);
		m_textureSize	= std::min(512, m_textureSize);
		std::cout << "Using texture size: " << m_textureSize << "." << std::endl;

		m_pow2	= glutExtensionSupported("GL_ARB_texture_non_power_of_two");
		std::cout << "Non Power of Two extension " << (m_pow2 ? "available." : "not available.") << std::endl;

		m_cb->Init();
		m_na->Init();
		m_progress->Init();

		m_initialImageLoading	= true;

		std::cout << std::endl;

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
	case PROP_BORDER:
		m_showBorder	= true;
		break;
	case PROP_RECURSIVE:
		m_recursiveDir	= true;
		break;
	}
}

void CWindow::SetProp(unsigned char r, unsigned char g, unsigned char b) {
	m_cb->SetColor(r, g, b);
}

void CWindow::fnRender() {
	if(m_initialImageLoading == true) {
		m_initialImageLoading	= false;
		loadImage(0);
	}

	m_cb->Render();

	glColor3f(1, 1, 1);

	if(m_na->Render() == false) {
		calculateScale();

		float img_w	= m_il->GetWidth() * m_scale;
		float img_h	= m_il->GetHeight() * m_scale;

		if(m_mouseLB == true || m_keyPressed == true) {
			m_keyPressed	= false;

			const int delta	= 20;
			m_imageDx	= std::max(m_imageDx, delta - (int)img_w);
			m_imageDx	= std::min(m_imageDx, m_curWinW - delta);
			m_imageDy	= std::max(m_imageDy, delta - (int)img_h);
			m_imageDy	= std::min(m_imageDy, m_curWinH - delta);
		}

		QuadsIc it = m_quads.begin(), itEnd = m_quads.end();
		for( ; it != itEnd; ++it) {
			CQuadImage* quad	= *it;

			float x	= m_imageDx + quad->GetCol() * quad->GetTexWidth() * m_scale;
			float y	= m_imageDy + quad->GetRow() * quad->GetTexHeight() * m_scale;

			float w	= quad->GetWidth() * m_scale;
			float h	= quad->GetHeight() * m_scale;
			quad->SetWindowSize(m_curWinW, m_curWinH);
			quad->RenderEx(x, y, w, h);
//			quad->RenderEx(x, y, w, h, 180);
		}

		if(m_showBorder == true) {
			m_border->Render((float)m_imageDx, (float)m_imageDy, img_w, img_h);
		}
	}

	m_ib->Render();

	glutSwapBuffers();
}

void CWindow::fnResize(int width, int height) {
	m_curWinW	= width;
	m_curWinH	= height - m_ib->GetHeight();

//	if(m_curWinW < DEF_WINDOW_W || m_curWinH < DEF_WINDOW_H) {
//		m_curWinW	= std::max(m_curWinW, DEF_WINDOW_W);
//		m_curWinH	= std::max(m_curWinH, DEF_WINDOW_H);
//		glutReshapeWindow(m_curWinW, m_curWinH);
//	}

	calculateScale();
	int w	= (int)(m_il->GetWidth() * m_scale);
	int h	= (int)(m_il->GetHeight() * m_scale);
	m_imageDx	= (m_curWinW - w) / 2;
	m_imageDy	= (m_curWinH - h) / 2;

	glViewport(0, 0, m_curWinW, m_curWinH + m_ib->GetHeight());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_curWinW, m_curWinH + m_ib->GetHeight(), 0, -1, 1);

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
			m_imageDx	+= diffx;
			m_imageDy	+= diffy;
			glutPostRedisplay();
		}
	}
}

void CWindow::fnMouseWheel(int wheel, int direction, int x, int y) {
	if(direction > 0) {
		updateScale(true);
	}
	else {//if(direction < 0) {
		updateScale(false);
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
//		exit(0);
		glutLeaveMainLoop();
		break;
	case 127:	// Delete
		if(mod == GLUT_ACTIVE_CTRL) {
			m_filesList->RemoveFromDisk();
		}
		break;
	case 'i':
	case 'I':
		m_ib->Show(!m_ib->Visible());
		fnResize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
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
	case 'b':
	case 'B':
		m_showBorder	= !m_showBorder;
		glutPostRedisplay();
		break;
	case '+':
	case '=':
		updateScale(true);
		break;
	case '-':
		updateScale(false);
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
		if(m_windowed == false) {
			m_prevWinX	= glutGet(GLUT_WINDOW_X);
			m_prevWinY	= glutGet(GLUT_WINDOW_Y);
			m_prevWinW	= glutGet(GLUT_WINDOW_WIDTH);
			m_prevWinH	= glutGet(GLUT_WINDOW_HEIGHT);
			glutPositionWindow(0, 0);
			glutReshapeWindow(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
//			glutFullScreen();
		}
		else {
			glutPositionWindow(m_prevWinX, m_prevWinY);
			glutReshapeWindow(m_prevWinW, m_prevWinH);
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
			m_imageDx	+= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_RIGHT:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_imageDx	-= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_UP:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_imageDy	+= 10;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_DOWN:
		if(m_fitImage == false) {
			m_keyPressed	= true;
			m_imageDy	-= 10;
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
        if(w >= m_curWinW || h >= m_curWinH) {
            float aspect  = (float)w / h;
            float nW = 0;
            float nH = 0;
            float dx	= (float)w / m_curWinW;
            float dy	= (float)h / m_curWinH;
            if(dx > dy) {
                if(w > m_curWinW) {
                    nW  = m_curWinW;
                    nH  = nW / aspect;
                }
            }
            else {
                if(h > m_curWinH) {
                    nH  = m_curWinH;
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

// TODO update m_imageDx / m_imageDy according current mouse position
void CWindow::updateScale(bool up) {
	m_fitImage	= false;

	int w	= m_il->GetWidth();
	int h	= m_il->GetHeight();

	float oldw	= w * m_scale;
	float oldh	= h * m_scale;

	if(up == true) {
		m_scale	/= 0.95f;
	}
	else {
		m_scale	*= 0.95f;
	}

	float neww	= w * m_scale;
	float newh	= h * m_scale;

	m_imageDx	+= (oldw - neww) / 2;
	m_imageDy	+= (oldh - newh) / 2;

	updateInfobar();
	glutPostRedisplay();
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
		m_na->Enable(false);
	}

	const char* path	= m_filesList->GetName(step);
	m_progress->Start();

	if(true == m_il->LoadImage(path, 0)) {
		createTextures();
		ret	= true;
	}
	else {
		m_na->Enable(true);
	}

	updateInfobar();

	m_il->FreeMemory();

	fnResize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//	centerWindow();

	return ret;
}

void CWindow::updateInfobar() {
	calculateScale();

	INFO_BAR s;
	const char* path	= m_filesList->GetName(0);
	s.path			= path;
	s.width			= m_il->GetWidth();
	s.height		= m_il->GetHeight();
	s.bpp			= m_il->GetImageBpp();
	s.scale			= m_scale;
	s.sub_image		= 0;//m_il->GetSub();
	s.sub_count		= 1;//m_il->GetSubCount();
	s.file_size		= m_il->GetSize();
	s.mem_size		= m_il->GetSizeMem();
	s.index			= m_filesList->GetIndex();
	s.files_count	= m_filesList->GetCount();

	m_ib->Update(&s);
}

void CWindow::calculateTextureSize(int* texW, int* texH, int imgW, int imgH) {
	int tw	= std::min(m_textureSize, imgW);
	int th	= std::min(m_textureSize, imgH);

    // correct texture size
    if(m_pow2 == false) {
		float power_w	= logf((float)tw) / logf(2.0f);
		float power_h	= logf((float)th) / logf(2.0f);
		if(static_cast<int>(power_w) != power_w || static_cast<int>(power_h) != power_h) {
			tw	= static_cast<int>(powf(2.0f, static_cast<int>(ceilf(power_w))));
			th	= static_cast<int>(powf(2.0f, static_cast<int>(ceilf(power_h))));
		}
    }
//	std::cout << "  select texture size: " << tw << " x " << th << std::endl;
	*texW	= tw;
	*texH	= th;
}

void CWindow::createTextures() {
	unsigned char* bitmap	= m_il->GetBitmap();
	int width	= m_il->GetWidth();
	int height	= m_il->GetHeight();
	int bpp		= m_il->GetBpp();
	int pitch	= m_il->GetPitch();
	int bytesPP	= (bpp / 8);

	std::cout << " " << width << " x " << height << ", ";

	int texW, texH;
	calculateTextureSize(&texW, &texH, width, height);
	// texture pitch should be multiple by 4
	int texPitch	= static_cast<int>(ceilf(texW * bytesPP / 4.0f) * 4);

	int cols	= static_cast<int>(ceilf(static_cast<float>(width) / texW));
	int rows	= static_cast<int>(ceilf(static_cast<float>(height) / texH));
	size_t quadsCount	= cols * rows;
	std::cout << "textures: " << quadsCount << " (" << cols << " x " << rows << ") required" << std::endl;

	deleteTextures();

	unsigned char* buffer	= new unsigned char[texPitch * texH];

	int idx	= 0;
	int height2	= height;
	for(int row = 0; row < rows; row++) {
		int width2	= width;
		int h	= (height2 > texH ? texH : height2);
		for(int col = 0; col < cols; col++) {
			int w	= (width2 > texW ? texW : width2);
			width2	-= w;

			int dx	= col * texPitch;
			int dy	= row * texH;
			int count	= w * bytesPP;
			for(int line = 0; line < h; line++) {
				int src	= dx + (dy + line) * pitch;
				int dst	= line * texPitch;
				memcpy(&buffer[dst], &bitmap[src], count);
			}

			CQuadImage* quad	= new CQuadImage(texW, texH, buffer, bpp);
			quad->SetCell(col, row);
			quad->SetSpriteSize(w, h);

			m_quads.push_back(quad);

			idx++;
		}
		height2	-= h;
	}

	delete[] buffer;
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

void CWindow::deleteTextures() {
	glBindTexture(GL_TEXTURE_2D, 0);
	QuadsIc it = m_quads.begin(), itEnd = m_quads.end();
	for( ; it != itEnd; ++it) {
		delete (*it);
	}
	m_quads.clear();
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


void CWindow::fnProgressLoading(int percent) {
	m_progress->Render(percent);
}

void CWindow::callbackProgressLoading(int percent) {
	g_window->fnProgressLoading(percent);
}
