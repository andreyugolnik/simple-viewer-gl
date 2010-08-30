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
        if(TIFFRGBAImageBegin(&img, tif, 0, NULL)) {
			m_width		= img.width;
			m_height	= img.height;
			m_pitch		= m_width * sizeof(uint32);
			m_sizeMem	= m_pitch * m_height;
			m_bitmap	= new unsigned char[m_sizeMem];
			m_bpp		= 32;
			m_bppImage	= img.bitspersample * img.samplesperpixel;
			m_format	= GL_RGBA;
			m_subCount	= TIFFNumberOfDirectories(tif);
			std::cout << " pages count: " << m_subCount;
			m_subImage	= std::min(subImage, m_subCount - 1);
			TIFFSetDirectory(tif, m_subImage);

			// left-to-right? top-to-bottom
			img.req_orientation	= 1;
			std::cout << " row: " << img.row_offset;
			std::cout << " col: " << img.col_offset;

			if(TIFFRGBAImageGet(&img, (uint32*)m_bitmap, m_width, m_height) == 0) {
				reset();
				TIFFClose(tif);
				return false;
			}
            TIFFRGBAImageEnd(&img);
        }
		else {
			reset();
			TIFFClose(tif);
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
