/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef PIXELINFO_H
#define PIXELINFO_H

#include "ftstring.h"
#include "quadseries.h"
#include "math/vector.h"
#include "math/rect.h"

#include <memory>

typedef struct PIXELINFO
{
    cVector img;
    cVector cursor;
    CRect<float> rc;
    int r, g, b, a;
    int w, h;//, pitch;
    //int x, y;	// pixel position
    //unsigned char* bitmap;
    //int bpp, format;
    //float scale;
} PixelInfo;

class CPixelInfo
{
public:
    CPixelInfo();
    virtual ~CPixelInfo();

    void Init();
    void Update(const PixelInfo* _p);
    void Render();
    void Show(bool show = true) { m_visible = show; }
    bool IsVisible() const { return m_visible; }
    void SetWindowSize(const cVector& _size) { m_window = _size; }
    void SetCursor(int cursor);

private:
    bool m_visible;
    cVector m_window;
    PixelInfo m_pixelInfo;
    std::auto_ptr<CQuad> m_bg;
    std::auto_ptr<CQuadSeries> m_pointer;
    std::auto_ptr<CFTString> m_ft;

private:
    bool checkBoundary() const;
};

#endif // PIXELINFO_H

