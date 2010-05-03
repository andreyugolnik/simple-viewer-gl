/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatgif.h"
#include <string.h>
#include <iostream>

CFormatGif::CFormatGif(Callback callback) : CFormat(callback) {
}

CFormatGif::~CFormatGif() {
	FreeMemory();
}

bool CFormatGif::Load(const char* filename, int sub_image) {
	if(openFile(filename) == false) {
		return false;
	}
	fclose(m_file);

	GifFileType* file	= DGifOpenFileName(filename);
	if(file == 0) {
		std::cout << "Error Opening GIF image" << std::endl;
		return false;
	}

	m_width		= file->SWidth;		//file->Image.Width;
	m_height	= file->SHeight;	//file->Image.Height;
	m_pitch		= file->SWidth * 3;
	m_bpp		= 24;
//	m_images	= file->ImageCount;
	m_bppImage	= 8;//file->Image.ColorMap->BitsPerPixel;
	m_bitmap	= new unsigned char[m_pitch * m_height];
	m_sizeMem	= m_pitch * m_height;

	if(ReadUntilImage(file) != GIF_ERROR) {
		ColorMapObject* ColorMap	= (file->Image.ColorMap ? file->Image.ColorMap : file->SColorMap);
		GifRowType GifRow	= new GifPixelType[m_width];
		for(int y = 0; y < m_height; y++) {
			if(DGifGetLine(file, GifRow, m_width) == GIF_ERROR) {
			}
			for(int x = 0; x < m_width; x++) {
				GifColorType* ColorMapEntry	= &ColorMap->Colors[GifRow[x]];
				m_bitmap[y * m_pitch + x * 3 + 0]	= ColorMapEntry->Red;
				m_bitmap[y * m_pitch + x * 3 + 1]	= ColorMapEntry->Green;
				m_bitmap[y * m_pitch + x * 3 + 2]	= ColorMapEntry->Blue;
			}

			int percent	= (int)(100.0f * y / m_height);
			progress(percent);
		}
		delete[] GifRow;
	}

	// UndefinedRecordType		- something is wrong!
	// ScreenDescRecordType	- screen information. As the screen info is automatically read in when the file is open, this should not happen.
	// ImageDescRecordType		- next record is an image descriptor.
	// ExtensionRecordType		- next record is extension block.
	// TerminateRecordType		- last record reached, can close the file.
//	GifRecordType recordType;
//	if(GIF_ERROR == DGifGetRecordType(file, &recordType)) {
//		DGifCloseFile(file);
//		std::cout << "Error Opening GIF image" << std::endl;
//		return false;
//	}
//
//	std::cout << "Record Type" << (int)recordType << std::endl;

	DGifCloseFile(file);

	return true;
}

void CFormatGif::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}

int CFormatGif::ReadUntilImage(GifFileType* file) {
    int ExtCode;
    GifRecordType RecordType;
    GifByteType* Extension;

    // Scan the content of the GIF file, until image descriptor is detected:
    do {
		if(DGifGetRecordType(file, &RecordType) == GIF_ERROR) {
			return GIF_ERROR;
		}

		switch(RecordType) {
		case IMAGE_DESC_RECORD_TYPE:
			return DGifGetImageDesc(file);

		case EXTENSION_RECORD_TYPE:
			// Skip any extension blocks in file:
			if(DGifGetExtension(file, &ExtCode, &Extension) == GIF_ERROR) {
				return GIF_ERROR;
			}

			while(Extension != NULL) {
				if(DGifGetExtensionNext(file, &Extension) == GIF_ERROR) {
					return GIF_ERROR;
				}
			}
			break;

		case TERMINATE_RECORD_TYPE:
			break;

		default:	// Should be traps by DGifGetRecordType
			break;
		}
    } while(RecordType != TERMINATE_RECORD_TYPE);

    return GIF_ERROR;	// We should be here - no image was found!
}
