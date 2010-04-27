/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef RECT_H
#define RECT_H


class CRect {
public:
    CRect();
    CRect(float x1, float y1, float x2, float y2);
    virtual ~CRect();

	bool IsSet() const { return m_isSet; }
	void Set(float x1, float y1, float x2, float y2);
    void Encapsulate(float x, float y);
    bool TestPoint(float x, float y) const;
    bool Intersect(const CRect* rc) const;

private:
	bool m_isSet;

public:
    float m_x1, m_y1, m_x2, m_y2;
};

#endif // RECT_H
