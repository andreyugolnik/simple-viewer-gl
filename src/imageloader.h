/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <memory>
#include <string>
#include <Imlib2.h>

typedef enum { ANGLE_0, ANGLE_90, ANGLE_180, ANGLE_270 } Angle;

class CImageLoader {
public:
	CImageLoader();
	virtual ~CImageLoader();

	bool LoadImage(const char* path, int sub_image);
	void SetAngle(Angle angle) { m_angle = angle; }

	unsigned char* GetBitmap() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetBpp() const;
	bool HasAlpha() const;
	long GetSize() const;
	const char* GetFormat() const { return m_format.c_str(); };
	int GetSub() const;
	int GetSubCount() const;

private:
	Imlib_Image m_image;
	Angle m_angle;
	std::string m_path;

	int m_width, m_height, m_bpp;
	long m_size;
	DATA32* m_data;
	bool m_alpha;
	std::string m_format;
};

#endif // IMAGELOADER_H
