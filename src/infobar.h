/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef INFOBAR_H
#define INFOBAR_H

#include <string>

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

private:
	bool m_visible;
	std::string m_bottominfo;
};

#endif // INFOBAR_H
