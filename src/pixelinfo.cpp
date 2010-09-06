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

const int BORDER	= 4;
const int ALPHA		= 200;
const int FONT_HEIGHT	= 13;
const int FRAME_DELTA	= 10;
const int LINES_COUNT	= 4;

CPixelInfo::CPixelInfo() : m_visible(false), m_windowWidth(0), m_windowHeight(0) {
	memset(&m_pixelInfo, 0, sizeof(m_pixelInfo));
}

CPixelInfo::~CPixelInfo() {
}

void CPixelInfo::Init() {
	m_bg.reset(new CQuad(0, 0));
	m_bg->SetColor(0, 0, 0, ALPHA);
	int format	= (imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
	m_pointer.reset(new CQuad(imgPointerCross.width, imgPointerCross.height, imgPointerCross.pixel_data, format));
	m_ft.reset(new CFTString(FONT_HEIGHT));
	m_ft->SetColor(255, 255, 255, ALPHA);
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

		info << "pos: " << p->x << " x " << p->y;
		info << "\nargb: " << std::hex << std::uppercase;
		info << std::setw(2) << std::setfill('0') << a;
		info << std::setw(2) << std::setfill('0') << r;
		info << std::setw(2) << std::setfill('0') << g;
		info << std::setw(2) << std::setfill('0') << b;

		info << "\nrect: " << std::dec;

		if(p->rc.IsSet() == true) {
			int x	= std::min(p->rc.x1, p->rc.x2);
			int y	= std::min(p->rc.y1, p->rc.y2);
			int w	= p->rc.GetWidth();
			int h	= p->rc.GetHeight();

			info << x << ", " << y << " -> " << x + w << ", " << y + h;
			info << "\nsize: " << (w + 1) << " x " << (h + 1);
		}
	}

	m_ft->Update(info.str().c_str());
}

void CPixelInfo::Render() {
	if(m_visible == true) {
		m_pointer->Render(m_pixelInfo.cursorx - 10, m_pixelInfo.cursory - 10);

		if(checkBoundary() == true) {
			int frameWidth	= m_ft->GetStringWidth() + 2 * BORDER;
			int frameHeight	= FONT_HEIGHT * LINES_COUNT + 2 * BORDER;

			int cursorx	= m_pixelInfo.cursorx + FRAME_DELTA;
			int cursory	= m_pixelInfo.cursory + FRAME_DELTA;
			if(cursorx > m_windowWidth - frameWidth) {
//				cursorx	= m_windowWidth - frameWidth;
				cursorx	= m_pixelInfo.cursorx - FRAME_DELTA - frameWidth;
			}
			if(cursory > m_windowHeight - frameHeight) {
//				cursory	= m_windowHeight - frameHeight;
				cursory	= m_pixelInfo.cursory - FRAME_DELTA - frameHeight;
			}

			m_bg->SetSpriteSize(frameWidth, frameHeight);
			m_bg->Render(cursorx, cursory);

			m_ft->Render(cursorx + BORDER, cursory + FONT_HEIGHT);
		}
	}
}

void CPixelInfo::SetWindowSize(int w, int h) {
	m_windowWidth	= w;
	m_windowHeight	= h;
}

bool CPixelInfo::checkBoundary() const {
	if(m_pixelInfo.bitmap != 0 && m_pixelInfo.scale == 1 &&
		m_pixelInfo.x >= 0 && m_pixelInfo.x < m_pixelInfo.w &&
		m_pixelInfo.y >= 0 && m_pixelInfo.y < m_pixelInfo.h) {

		return true;
	}

	return false;
}
