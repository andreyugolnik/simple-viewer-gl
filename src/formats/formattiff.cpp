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

	TIFF* tif	= TIFFOpen(filename, "r");
    if(tif != 0) {
        TIFFRGBAImage img;
        char emsg[1024];
        
        if(TIFFRGBAImageBegin(&img, tif, 0, emsg)) {
			m_width		= img.width;
			m_height	= img.height;
			m_pitch		= m_width * sizeof(uint32);
			m_sizeMem	= m_pitch * m_height;
			m_bitmap	= new unsigned char[m_sizeMem];
			m_bpp		= 32;
			m_bppImage	= 32;
			m_format	= GL_RGBA;

			if(TIFFRGBAImageGet(&img, (uint32*)m_bitmap, m_width, m_height) == 0) {
				reset();
				return false;
			}
            TIFFRGBAImageEnd(&img);
        }
		else {
            TIFFError(filename, emsg);
			reset();
			return false;
		}
        TIFFClose(tif);
    }

	return true;
}

void CFormatTiff::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}
