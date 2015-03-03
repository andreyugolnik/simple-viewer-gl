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

class CProgress
{
public:
    CProgress();
    virtual ~CProgress();

    void Init();
    void Start();
    void Render(int percent);

private:
    int m_loadingTime;
    std::unique_ptr<CQuad> m_loading;
    std::unique_ptr<CQuad> m_square;
};

#endif // PROGRESS_H

