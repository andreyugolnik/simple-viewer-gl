/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATICO_H
#define FORMATICO_H

#include "format.h"

namespace FORMAT_ICO {
	#pragma pack(push, 1)
	typedef struct ICO_HEADER {
		uint16 reserved;	// Reserved. Should always be 0.
		uint16 type;		// Specifies image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid.
		uint16 count;		// Specifies number of images in the file.
	} IcoHeader;

	typedef struct ICO_IMAGE {
		uint8 width;	// Specifies image width in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image width is 256 pixels.
		uint8 height;	// Specifies image height in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image height is 256 pixels.
		uint8 colors;	// Specifies number of colors in the color palette. Should be 0 if the image is truecolor.
		uint8 reserved;	// Reserved. Should be 0.[Notes 1]
		uint16 planes;	// In .ICO format: Specifies color planes. Should be 0 or 1.[Notes 2]
						// In .CUR format: Specifies the horizontal coordinates of the hotspot in number of pixels from the left.
		uint16 bits;	// In .ICO format: Specifies bits per pixel. [Notes 3]
						// In .CUR format: Specifies the vertical coordinates of the hotspot in number of pixels from the top.
		uint32 size;	// Specifies the size of the bitmap data in bytes
		uint32 offset;	// Specifies the offset of bitmap data address in the file
	} IcoImage;
	#pragma pack(pop)
};

class CFormatIco : public CFormat {
public:
	CFormatIco(Callback callback);
	virtual ~CFormatIco();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

private:
};

#endif // FORMATICO_H
