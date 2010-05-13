/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatico.h"
#include <string.h>

using namespace FORMAT_ICO;

CFormatIco::CFormatIco(Callback callback) : CFormat(callback) {
}

CFormatIco::~CFormatIco() {
	FreeMemory();
}

bool CFormatIco::Load(const char* filename, int sub_image) {
	if(openFile(filename) == false) {
		return false;
	}

	IcoHeader header;
	if(1 != fread(&header, sizeof(IcoHeader), 1, m_file)) {
		fclose(m_file);
		return false;
	}

	IcoImage* image	= new IcoImage[header.images];
	if(header.images != fread(image, sizeof(IcoImage), header.images, m_file)) {
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
