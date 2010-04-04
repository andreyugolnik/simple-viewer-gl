/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include "quad.h"
#include <memory>

class CCheckerboard {
public:
	CCheckerboard();
	virtual ~CCheckerboard();

	void Init();
	void Render();
	void Enable(bool enable = true) { m_enabled = enable; }
	bool IsEnabled() const { return m_enabled; }
	void SetColor(unsigned char r, unsigned char g, unsigned char b);

private:
	bool m_enabled;
	GLfloat m_r, m_g, m_b;
	std::auto_ptr<CQuad> m_cb;
};

#endif // CHECKERBOARD_H
