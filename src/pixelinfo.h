/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef PIXELINFO_H
#define PIXELINFO_H

#include "ftstring.h"
#include "quadseries.h"
#include "math/vector.h"
#include "math/rect.h"

#include <memory>

struct sPixelInfo
{
    cVector mouse;
    cVector point;
    int img_w, img_h;
    unsigned char r, g, b, a;
    CRect<float> rc;
};

class CPixelInfo
{
public:
    CPixelInfo();
    virtual ~CPixelInfo();

    void Init();
    void setPixelInfo(const sPixelInfo& pi);
    void Render();
    void Show(bool show = true) { m_visible = show; }
    bool IsVisible() const { return m_visible; }
    void SetWindowSize(const cVector& size) { m_window = size; }
    void SetCursor(int cursor);

private:
    bool m_visible;
    cVector m_window;
    sPixelInfo m_pixelInfo;
    std::auto_ptr<CQuad> m_bg;
    std::auto_ptr<CQuadSeries> m_pointer;
    std::auto_ptr<CFTString> m_ft;

private:
    bool isInsideImage(const cVector& pos) const;
};

#endif // PIXELINFO_H

