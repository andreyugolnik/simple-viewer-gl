/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef PROGRESS_H
#define PROGRESS_H

#include "quad.h"
#include <memory>

class CProgress {
public:
	CProgress();
	virtual ~CProgress();

	void Init();
	void Start();
	void Render();

private:
	int m_loadingTime;
	int m_progress;
	std::auto_ptr<CQuad> m_loading;
	std::auto_ptr<CQuad> m_square;
};

#endif // PROGRESS_H
