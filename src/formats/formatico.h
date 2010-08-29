/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATICO_H
#define FORMATICO_H

#include "format.h"
#include <png.h>

namespace FORMAT_ICO {
	#pragma pack(push, 1)
	typedef struct ICO_HEADER {
		uint16 reserved;	// Reserved. Should always be 0.
		uint16 type;		// Specifies image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid.
		uint16 count;		// Specifies number of images in the file.
	} IcoHeader;

	// List of icons.
	// Size = IcoHeader.ount * 16
	typedef struct ICO_DIRENTRY {
		uint8 width;	// Specifies image width in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image width is 256 pixels.
		uint8 height;	// Specifies image height in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image height is 256 pixels.
		uint8 colors;	// Specifies number of colors in the color palette. Should be 0 if the image is truecolor.
		uint8 reserved;	// Reserved. Should be 0.[Notes 1]
		uint16 planes;	// In .ICO format: Specifies color planes. Should be 0 or 1.
						// In .CUR format: Specifies the horizontal coordinates of the hotspot in number of pixels from the left.
		uint16 bits;	// In .ICO format: Specifies bits per pixel. (1, 4, 8)
						// In .CUR format: Specifies the vertical coordinates of the hotspot in number of pixels from the top.
		uint32 size;	// Specifies the size of the bitmap data in bytes. Size of (InfoHeader + ANDbitmap + XORbitmap)
		uint32 offset;	// Specifies the offset of bitmap data address in the file
	} IcoDirentry;

	// Variant of BMP InfoHeader.
	// Size = 40 bytes.
	typedef struct ICO_BMP_INFOHEADER {
		uint32 size;		// Size of InfoHeader structure = 40
		uint32 width;		// Icon Width
		uint32 height;		// Icon Height (added height of XOR-Bitmap and AND-Bitmap)
		uint16 planes;		// number of planes = 1
		uint16 bits;		// bits per pixel = 1, 2, 4, 8, 16, 24, 32
		uint32 reserved0;	// Type of Compression = 0
		uint32 imagesize;	// Size of Image in Bytes = 0 (uncompressed)
		uint32 reserved1;	// XpixelsPerM
		uint32 reserved2;	// YpixelsPerM
		uint32 reserved3;	// ColorsUsed
		uint32 reserved4;	// ColorsImportant
	} IcoBmpInfoHeader;

	// Color Map for XOR-Bitmap.
	// Size = NumberOfColors * 4 bytes.
	typedef struct ICO_COLORS {
		uint8 red;		// red component
		uint8 green;	// green component
		uint8 blue;		// blue component
		uint8 reserved;	// = 0
	} IcoColors;

//		uint8 xormask;	// DIB bits for XOR mask
//		uint8 andmask;	// DIB bits for AND mask
	#pragma pack(pop)

	typedef struct PNG_RAW {
		uint8* data;
		size_t size;	// size of raw data
		size_t pos;	// current pos
	} PngRaw;
};

class CFormatIco : public CFormat {
public:
	CFormatIco(Callback callback);
	virtual ~CFormatIco();

	virtual bool Load(const char* filename, int subImage = 0);
	virtual void FreeMemory();

private:
	static FORMAT_ICO::PngRaw m_pngRaw;

private:
	bool loadOrdinaryFrame(const FORMAT_ICO::IcoDirentry* image);
	static void readPngData(png_structp png, png_bytep out, png_size_t count);
	bool loadPngFrame(const FORMAT_ICO::IcoDirentry* image);
	int calcIcoPitch();
	int getBit(const uint8* data, int bit);
	int getNibble(const uint8* data, int nibble);
	int getByte(const uint8* data, int byte);
};

#endif // FORMATICO_H
