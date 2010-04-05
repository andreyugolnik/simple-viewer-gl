/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef INFOBAR_H
#define INFOBAR_H

#include "quad.h"
#include <string>
#include <memory>

typedef struct INFO_BAR {
	const char* path;
	int index;
	int width, height, bpp;
	float scale;
	int sub_image, sub_count;
	long file_size;
	int files_count;
} InfoBar;

class CInfoBar {
public:
	CInfoBar();
	virtual ~CInfoBar();

	void Render();
	bool Visible() { return m_visible; }
	void Show(bool show = true) { m_visible = show; }
	void Update(const InfoBar* p);
	float GetHeight() { return (m_visible == true ? m_height : 0); }

private:
	bool m_visible;
	float m_height;
	std::string m_bottominfo;
	std::auto_ptr<CQuad> m_bg;
};

#endif // INFOBAR_H
