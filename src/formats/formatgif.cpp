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

//	ReadUntilImage(file);

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

	m_width		= file->SWidth;		//file->Image.Width;
	m_height	= file->SHeight;	//file->Image.Height;
	m_pitch		= 0;
	m_bpp		= 0;
	// get real bits per pixel
//	m_images	= file->ImageCount;
	m_bppImage	= 0;//file->Image.ColorMap->BitsPerPixel;
	m_bitmap	= 0;//new unsigned char[m_pitch * m_height];
	m_sizeMem	= 0;

	DGifCloseFile(file);

	return true;
}

void CFormatGif::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}

int CFormatGif::ReadUntilImage(GifFileType *GifFile) {
    int ExtCode;
    GifRecordType RecordType;
    GifByteType* Extension;

    /* Scan the content of the GIF file, until image descriptor is detected: */
    do {
		if(DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR)
			return GIF_ERROR;

		switch (RecordType) {
		case IMAGE_DESC_RECORD_TYPE:
			return DGifGetImageDesc(GifFile);
		case EXTENSION_RECORD_TYPE:
			/* Skip any extension blocks in file: */
			if(DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR)
				return GIF_ERROR;

			while (Extension != NULL)
				if(DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR)
					return GIF_ERROR;
			break;
		case TERMINATE_RECORD_TYPE:
			break;
		default:		     /* Should be traps by DGifGetRecordType */
			break;
		}
    }
    while(RecordType != TERMINATE_RECORD_TYPE);

    return GIF_ERROR;		  /* We should be here - no image was found! */
}
