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
	void Set(CRect* rc);
	void SetLeftTop(float x, float y);
	void SetRightBottom(float x, float y);
	void Encapsulate(float x, float y);
	bool TestPoint(float x, float y) const;
	bool Intersect(const CRect* rc) const;
	float GetWidth() const;
	float GetHeight() const;

private:
	bool m_isSet;

public:
    union {
		float m_x1;
		float x1;
	};
    union {
		float m_y1;
		float y1;
	};
    union {
		float m_x2;
		float x2;
	};
    union {
		float m_y2;
		float y2;
	};
};

#endif // RECT_H
