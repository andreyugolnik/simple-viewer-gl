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

	IcoImage* images	= new IcoImage[header.images];
	if(header.images != fread(images, sizeof(IcoImage), header.images, m_file)) {
		delete[] images;
		fclose(m_file);
		return false;
	}

	IcoImage* image	= &images[sub_image];
	m_width		= image->width == 0 ? 256 : image->width;
	m_height	= image->height == 0 ? 256 : image->height;
	m_pitch		= m_width * m_height * 4;
	m_bpp		= 32;
	m_bppImage	= image->bits;
	m_bitmap	= new uint8[m_pitch * m_height];
	m_sizeMem	= m_pitch * m_height;

	fseek(m_file, image->offset, SEEK_SET);
	uint8* p	= new uint8[image->size];
	if(1 != fread(p, image->size, 1, m_file)) {
		delete[] p;
		delete[] images;
		fclose(m_file);
		return false;
	}
	delete[] p;
	std::cout << "data size: " << image->size << " bytes." << std::endl;

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

	delete[] images;
	fclose(m_file);

	return true;
}

void CFormatIco::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}
