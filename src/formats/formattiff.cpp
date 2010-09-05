/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formattiff.h"
#include <string.h>
#include <tiffio.h>

CFormatTiff::CFormatTiff(Callback callback) : CFormat(callback) {
}

CFormatTiff::~CFormatTiff() {
	FreeMemory();
}

bool CFormatTiff::Load(const char* filename, int subImage) {
	if(openFile(filename) == false) {
		return false;
	}
	fclose(m_file);

	bool ret	= false;

	TIFF* tif	= TIFFOpen(filename, "r");
    if(tif != 0) {
		// read count of pages in image
		m_subCount	= TIFFNumberOfDirectories(tif);
		m_subImage	= std::min(subImage, m_subCount - 1);

		// set desired page
		if(TIFFSetDirectory(tif, m_subImage) != 0) {
			TIFFRGBAImage img;
			if(TIFFRGBAImageBegin(&img, tif, 0, NULL) != 0) {
				m_width		= img.width;
				m_height	= img.height;
				m_pitch		= m_width * sizeof(uint32);
				m_sizeMem	= m_pitch * m_height;
				m_bitmap	= new unsigned char[m_sizeMem];
				m_bpp		= 32;
				m_bppImage	= img.bitspersample * img.samplesperpixel;
				m_format	= GL_RGBA;

				// set desired orientation
				img.req_orientation	= ORIENTATION_TOPLEFT;

				if(TIFFRGBAImageGet(&img, (uint32*)m_bitmap, m_width, m_height) != 0) {
					ret	= true;
				}
				TIFFRGBAImageEnd(&img);
			}
		}
    }

	if(tif != 0) {
		TIFFClose(tif);
	}

	// clean if error
	if(ret == false) {
		reset();
	}

	return ret;
}

void CFormatTiff::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}