/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "formats/format.h"
#include <memory>
#include <string>

typedef enum { ANGLE_0, ANGLE_90, ANGLE_180, ANGLE_270 } Angle;

class CImageLoader {
public:
	CImageLoader(Callback callback);
	virtual ~CImageLoader();

	bool LoadImage(const char* path, int sub_image);
	void SetAngle(Angle angle) { m_angle = angle; }

	unsigned char* GetBitmap() const;
	void FreeMemory();
	int GetWidth() const;
	int GetHeight() const;
	int GetBpp() const;
	int GetImageBpp() const;
	long GetSize() const;
//	int GetSub() const;
//	int GetSubCount() const;

private:
	Angle m_angle;
	Callback m_callback;
	std::string m_path;
	std::auto_ptr<CFormat> m_image;

private:
	enum FILE_FORMAT { FORMAT_JPEG, FORMAT_COMMON } ;
	struct FORMAT {
		const char* ext;
		int format;
	};
	int getFormat();
};

#endif // IMAGELOADER_H
