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

	while(ReadUntilImage(file) != GIF_ERROR) {
		ColorMapObject* ColorMap	= (file->Image.ColorMap ? file->Image.ColorMap : file->SColorMap);
		GifRowType GifRow	= new GifPixelType[file->Image.Width];

		if(file->Image.Interlace) {
			// Need to perform 4 passes on the images:
			int InterlacedOffset[]	= { 0, 4, 2, 1 };	// The way Interlaced image should
			int InterlacedJumps[]	= { 8, 8, 4, 2 };	// be read - offsets and jumps...

			for(int i = 0; i < 4; i++) {
				for(int y = InterlacedOffset[i]; y < file->Image.Height; y += InterlacedJumps[i]) {
					if(DGifGetLine(file, GifRow, file->Image.Width) == GIF_ERROR) {
					}

					for(int x = 0; x < file->Image.Width; x++) {
						GifColorType* ColorMapEntry	= &ColorMap->Colors[GifRow[x]];
						int pos	= (y + file->Image.Top) * m_pitch + (x + file->Image.Left) * 3;
						m_bitmap[pos + 0]	= ColorMapEntry->Red;
						m_bitmap[pos + 1]	= ColorMapEntry->Green;
						m_bitmap[pos + 2]	= ColorMapEntry->Blue;
					}

					int percent	= (int)(100.0f * y / file->Image.Height);
					progress(percent);
				}
			}
		}
		else {
			for(int y = 0; y < file->Image.Height; y++) {
				if(DGifGetLine(file, GifRow, file->Image.Width) == GIF_ERROR) {
				}

				for(int x = 0; x < file->Image.Width; x++) {
					GifColorType* ColorMapEntry	= &ColorMap->Colors[GifRow[x]];
					int pos	= (y + file->Image.Top) * m_pitch + (x + file->Image.Left) * 3;
					m_bitmap[pos + 0]	= ColorMapEntry->Red;
					m_bitmap[pos + 1]	= ColorMapEntry->Green;
					m_bitmap[pos + 2]	= ColorMapEntry->Blue;
				}

				int percent	= (int)(100.0f * y / file->Image.Height);
				progress(percent);
			}
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
