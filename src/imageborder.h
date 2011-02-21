/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGEBORDER_H
#define IMAGEBORDER_H

class CImageBorder
{
public:
    CImageBorder();
    virtual ~CImageBorder();

    void SetColor(int r, int g, int b, int a);
    void Render(int x, int y, int w, int h);
    int GetBorderWidth() const;

private:
    int m_r, m_g, m_b, m_a;

private:
    void renderLine(float x1, float y1, float x2, float y2);
};

#endif // IMAGEBORDER_H

