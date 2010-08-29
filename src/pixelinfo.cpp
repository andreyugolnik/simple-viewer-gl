/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "pixelinfo.h"
#include "img-pointer-cross.c"

#include <sstream>
#include <iomanip>

const int border	= 4;
const int alpha		= 200;
const int fontHeight	= 14;
const int frameDelta	= 10;

CPixelInfo::CPixelInfo() : m_visible(false), m_windowWidth(0), m_windowHeight(0) {
	memset(&m_pixelInfo, 0, sizeof(m_pixelInfo));
}

CPixelInfo::~CPixelInfo() {
}

void CPixelInfo::Init() {
	m_bg.reset(new CQuad(0, 0));
	int format	= (imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
	m_pointer.reset(new CQuad(imgPointerCross.width, imgPointerCross.height, imgPointerCross.pixel_data, format));
	m_ft.reset(new CFTString(fontHeight));
}

void CPixelInfo::Update(const PixelInfo* p) {
	memcpy(&m_pixelInfo, p, sizeof(m_pixelInfo));

	std::stringstream info;

	// TODO correct cursor position according scale factor
	if(checkBoundary() == true) {
		// TODO check pixel format (RGB or BGR)
		unsigned char* color	= p->bitmap + p->x * p->bpp / 8 + p->y * p->pitch;
		int a	= p->bpp == 32 ? color[3] : 255;
		int r	= color[0];
		int g	= color[1];
		int b	= color[2];

		info << "x: " << p->x << "\ny: " << p->y;
		info << "\nargb: " << std::hex << std::uppercase;
		info << std::setw(2) << std::setfill('0') << a;
		info << std::setw(2) << std::setfill('0') << r;
		info << std::setw(2) << std::setfill('0') << g;
		info << std::setw(2) << std::setfill('0') << b;
	}

	m_ft->Update(info.str().c_str());
}

void CPixelInfo::Render() {
	if(m_visible == true) {
		m_pointer->Render(m_pixelInfo.cursorx - 10, m_pixelInfo.cursory - 10);

		if(checkBoundary() == true) {
			int frameWidth	= m_ft->GetStringWidth() + 2 * border;
			int frameHeight	= fontHeight * 3 + 2 * border;

			int cursorx	= m_pixelInfo.cursorx + frameDelta;
			int cursory	= m_pixelInfo.cursory + frameDelta;
			if(cursorx > m_windowWidth - frameWidth) {
//				cursorx	= m_windowWidth - frameWidth;
				cursorx	= m_pixelInfo.cursorx - frameDelta - frameWidth;
			}
			if(cursory > m_windowHeight - frameHeight) {
//				cursory	= m_windowHeight - frameHeight;
				cursory	= m_pixelInfo.cursory - frameDelta - frameHeight;
			}

			glColor4ub(0, 0, 0, alpha);
			m_bg->SetSpriteSize(frameWidth, frameHeight);
			m_bg->Render(cursorx, cursory);

			glColor4ub(255, 255, 255, alpha);
			m_ft->Render(cursorx + border, cursory + fontHeight);
		}
	}
}

void CPixelInfo::SetWindowSize(int w, int h) {
	m_windowWidth	= w;
	m_windowHeight	= h;
}

bool CPixelInfo::checkBoundary() const {
	if(m_pixelInfo.scale == 1 &&
		m_pixelInfo.x >= 0 && m_pixelInfo.x < m_pixelInfo.w &&
		m_pixelInfo.y >= 0 && m_pixelInfo.y < m_pixelInfo.h) {

		return true;
	}

	return false;
}
