/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatico.h"
#include <string.h>

CFormatIco::CFormatIco(Callback callback) : CFormat(callback) {
}

CFormatIco::~CFormatIco() {
	FreeMemory();
}

#pragma pack(push, 1)
typedef struct ICO_HEADER {
	uint16 reserved;	// Reserved. Should always be 0.
	uint16 type;		// Specifies image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid.
	uint16 images;		// Specifies number of images in the file.
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

bool CFormatIco::Load(const char* filename, int sub_image) {
	if(openFile(filename) == false) {
		return false;
	}

	ICO_HEADER header;
	if(1 != fread(&header, sizeof(header), 1, m_file)) {
		fclose(m_file);
		return false;
	}

	ICO_IMAGE* image	= new ICO_IMAGE[header.images];
	if(header.images != fread(image, sizeof(ICO_IMAGE), header.images, m_file)) {
		delete[] image;
		fclose(m_file);
		return false;
	}

	m_width		= image->width;
	m_height	= image->height;
	m_pitch		= m_width * m_height * 4;
	m_bpp		= 32;
	m_bppImage	= image->bits;
	m_bitmap	= new unsigned char[m_pitch * m_height];
	m_sizeMem	= m_pitch * m_height;

//	unsigned char* p	= m_bitmap;
//	while(cinfo.output_scanline < cinfo.output_height) {
//		/* jpeg_read_scanlines expects an array of pointers to scanlines.
//		 * Here the array is only one element long, but you could ask for
//		 * more than one scanline at a time if that's more convenient.
//		 */
//		jpeg_read_scanlines(&cinfo, &p, 1);
//		p	+= row_stride;
//
//		int percent	= (int)(100.0f * cinfo.output_scanline / cinfo.output_height);
//		progress(percent);
//	}

	delete[] image;
	fclose(m_file);

	return true;
}

void CFormatIco::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}
