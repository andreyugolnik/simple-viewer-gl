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

bool CFormatGif::Load(const char* filename, int subImage) {
	if(openFile(filename) == false) {
		return false;
	}
	fclose(m_file);

	GifFileType* file	= DGifOpenFileName(filename);
	if(file == 0) {
		std::cout << "Error Opening GIF image" << std::endl;
		return false;
	}

	int res	= DGifSlurp(file);
	if(res != GIF_OK || file->ImageCount < 1) {
		std::cout << "Error Opening GIF image" << std::endl;
		return false;
	}

	subImage	= std::max(subImage, 0);
	subImage	= std::min(subImage, file->ImageCount - 1);
	m_subImage	= subImage;
	m_subCount	= file->ImageCount;

	m_width		= file->SWidth;		//file->Image.Width;
	m_height	= file->SHeight;	//file->Image.Height;
	m_pitch		= file->SWidth * 4;
	m_bpp		= 32;
	m_bppImage	= 8;//file->Image.ColorMap->BitsPerPixel;
	m_bitmap	= new unsigned char[m_pitch * m_height];
	memset(m_bitmap, 0, m_pitch * m_height);
	m_sizeMem	= m_pitch * m_height;


	const SavedImage* image		= &file->SavedImages[subImage];

	// look for the transparent color extension
	int	transparent	= -1;
	for(int i = 0; i < image->ExtensionBlockCount; i++) {
		ExtensionBlock* eb	= image->ExtensionBlocks + i;
		if(eb->Function == 0xF9 && eb->ByteCount == 4) {
			bool has_transparency	= ((eb->Bytes[0] & 1) == 1);
			if(has_transparency) {
				transparent	= eb->Bytes[3];
			}
		}
	}

	const ColorMapObject* cmap	= image->ImageDesc.ColorMap;
	if(cmap == 0) {
		cmap	= file->SColorMap;
	}
//	if(cmap->ColorCount != (1 << cmap->BitsPerPixel)) {
//		// error
//	}
	const int width	= image->ImageDesc.Width;
	const int height	= image->ImageDesc.Height;

	if(image->ImageDesc.Interlace) {
		// Need to perform 4 passes on the images:
		int interlacedOffset[]	= { 0, 4, 2, 1 };	// The way Interlaced image should
		int interlacedJumps[]	= { 8, 8, 4, 2 };	// be read - offsets and jumps...
		int interlaced_y	= 0;

		for(int i = 0; i < 4; i++) {
			for(int y = interlacedOffset[i]; y < height; y += interlacedJumps[i]) {

				for(int x = 0; x < width; x++) {
					int index	= image->RasterBits[interlaced_y * width + x];
					int pos	= (y + image->ImageDesc.Top) * m_pitch + (x + image->ImageDesc.Left) * 4;
					m_bitmap[pos + 0]	= cmap->Colors[index].Red;
					m_bitmap[pos + 1]	= cmap->Colors[index].Green;
					m_bitmap[pos + 2]	= cmap->Colors[index].Blue;
					m_bitmap[pos + 3]	= (transparent == index ? 0 : 255);
				}

				int percent	= (int)(100.0f * interlaced_y / height);
				progress(percent);

				interlaced_y++;
			}
		}
	}
	else {
		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				int index	= image->RasterBits[y * width + x];
				int pos	= (y + image->ImageDesc.Top) * m_pitch + (x + image->ImageDesc.Left) * 4;
				m_bitmap[pos + 0]	= cmap->Colors[index].Red;
				m_bitmap[pos + 1]	= cmap->Colors[index].Green;
				m_bitmap[pos + 2]	= cmap->Colors[index].Blue;
				m_bitmap[pos + 3]	= (transparent == index ? 0 : 255);
			}

			int percent	= (int)(100.0f * y / height);
			progress(percent);
		}
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

int CFormatGif::readUntilImage(GifFileType* file) {
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
