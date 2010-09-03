/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef PIXELINFO_H
#define PIXELINFO_H

#include "ftstring.h"
#include "quad.h"
#include <memory>

typedef struct PIXELINFO {
	int cursorx, cursory;
	int x, y;	// pixel position
	unsigned char* bitmap;
	int w, h, pitch;
	int bpp, format;
	CRect<int> rc;
	float scale;
} PixelInfo;

class CPixelInfo {
public:
	CPixelInfo();
	virtual ~CPixelInfo();

	void Init();
	void Update(const PixelInfo* p);
	void Render();
	void Show(bool show = true) { m_visible = show; }
	bool IsVisible() const { return m_visible; }
	void SetWindowSize(int w, int h);

private:
	bool m_visible;
	int m_windowWidth, m_windowHeight;
	PixelInfo m_pixelInfo;
	std::auto_ptr<CQuad> m_bg;
	std::auto_ptr<CQuad> m_pointer;
	std::auto_ptr<CFTString> m_ft;

private:
	bool checkBoundary() const;
};

#endif // PIXELINFO_H
