/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatpsd.h"
#include <string.h>
#include <iostream>

using namespace FORMAT_PSD;

CFormatPsd::CFormatPsd(Callback callback) : CFormat(callback), m_buffer(0), m_linesLengths(0) {
	memset(m_chBufs, 0, sizeof(m_chBufs));
}

CFormatPsd::~CFormatPsd() {
	FreeMemory();
}

bool CFormatPsd::Load(const char* filename, int sub_image) {
	if(openFile(filename) == false) {
		return false;
	}

	PSD_HEADER header;

	if(sizeof(PSD_HEADER) != fread(&header, 1, sizeof(PSD_HEADER), m_file)) {
		std::cout << "Can't read PSD header" << std::endl;
		cleanup();
		return false;
	}

	if(header.signature[0] != '8' || header.signature[1] != 'B' || header.signature[2] != 'P' || header.signature[3] != 'S') {
		std::cout << "Not valid PSD file" << std::endl;
		cleanup();
		return false;
	}

	int color_mode	= read_uint16((uint8*)&header.color_mode);
	if(color_mode != PSD_MODE_RGB && color_mode != PSD_MODE_CMYK) {
		std::cout << "Unsupported color mode: " << color_mode << std::endl;
		cleanup();
		return false;
	}

	int depth	= read_uint16((uint8*)&header.depth);
	if(depth != 8) {
		std::cout << "Unsupported depth: " << depth << std::endl;
		cleanup();
		return false;
	}

	int channels	= read_uint16((uint8*)&header.channels);
	if(channels != 3 && channels != 4) {
//		std::cout << "Unsupported cannels count: " << channels << std::endl;
//		cleanup();
//		return false;
	}
	std::cout << "Extra channels count: " << (channels >= 4 ? channels - 4 : channels) << std::endl;

	// skip Color Mode Data Block
	if(false == skipNextBlock()) {
		std::cout << "Can't read Color Mode Data Block" << std::endl;
		cleanup();
		return false;
	}

	// skip Image Resources Block
	if(false == skipNextBlock()) {
		std::cout << "Can't read Image Resources Block" << std::endl;
		cleanup();
		return false;
	}

	// Layer and Mask Information Block
	if(false == skipNextBlock()) {
		std::cout << "Can't read Layer and Mask Information Block" << std::endl;
		cleanup();
		return false;
	}

	// Image Data Block
	uint16 compression;
	if(sizeof(uint16) != fread(&compression, 1, sizeof(uint16), m_file)) {
		std::cout << "Can't read compression info" << std::endl;
		cleanup();
		return false;
	}
	compression	= read_uint16((uint8*)&compression);

	m_width		= read_uint32((uint8*)&header.columns);
	m_height	= read_uint32((uint8*)&header.rows);

	// this will be needed for RLE decompression
	m_linesLengths	= new uint16[channels * m_height];
	for(int i = 0; i < channels; i++) {
		int pos	= m_height * i;

		if(m_height * sizeof(uint16) != fread(&m_linesLengths[pos], 1, m_height * sizeof(uint16), m_file)) {
			std::cout << "Can't read length of lines" << std::endl;
			cleanup();
			return false;
		}

		// convert from different endianness
		for(int a = 0; a < m_height; a++) {
			m_linesLengths[pos + a]	= read_uint16((uint8*)&m_linesLengths[pos + a]);
		}
	}

	// !!!! this is a temporal hack, need more investigation !!!
	// read only first 3 or 4 channels
	channels	= std::min(channels, 4);

	m_bpp		= depth * channels;
	m_bppImage	= m_bpp;

	// we need buffer that can contain one channel data of one
	// row in RLE compressed format. 2*width should be enough
	m_buffer	= new uint8[m_width * 2];

	// create separate buffers for each channel (up to 24 buffers by spec)
	for(int i = 0; i < channels; i++) {
		m_chBufs[i]	= new uint8[m_width * m_height];
	}

	// read all channels rgba and extra if available;
	int currentRow		= 0;
	int currentChannel	= 0;
	int pos				= 0;
	bool done			= false;
	do {
		size_t lineLength	= m_width;
		if(compression == 1) {
			lineLength	= m_linesLengths[currentChannel * m_height + currentRow];
		}

		if(lineLength != fread(m_buffer, 1, lineLength, m_file)) {
			std::cout << "Error reading Image Data Block" << std::endl;
			cleanup();
			return false;
		}

		int percent	= (int)(100.0f * (currentChannel * m_height + currentRow) / (channels * m_height));
		progress(percent);

		if(compression == 1) {
			decompressLine(m_buffer, lineLength, m_chBufs[currentChannel] + pos);
		}
		else {
			memcpy(m_chBufs[currentChannel] + pos, m_buffer, m_width);
		}

		pos	+= m_width;
		currentRow++;

		if(currentRow == m_height) {
			currentRow	= 0;
			pos			= 0;
			currentChannel++;
			if(currentChannel == channels) {
				done	= true;
			}
		}
	} while(done == false);

	// convert or copy channel buffers to BGR / BGRA
	m_pitch		= m_width * channels;
	m_bitmap	= new unsigned char[m_pitch * m_height];
	unsigned char* bitmap	= m_bitmap;

	if(color_mode == PSD_MODE_RGB) {
		if(channels == 3) {
			for(int y = 0; y < m_height; y++) {
				for (int x = 0; x < m_width; x++) {
					bitmap[0]	= *(m_chBufs[2] + m_width * y + x);
					bitmap[1]	= *(m_chBufs[1] + m_width * y + x);
					bitmap[2]	= *(m_chBufs[0] + m_width * y + x);
					bitmap	+= 3;
				}
			}
		}
		else if(channels == 4) {
			for(int y = 0; y < m_height; y++) {
				for(int x = 0; x < m_width; x++) {
					bitmap[0]	= *(m_chBufs[2] + m_width * y + x);
					bitmap[1]	= *(m_chBufs[1] + m_width * y + x);
					bitmap[2]	= *(m_chBufs[0] + m_width * y + x);
					bitmap[3]	= *(m_chBufs[3] + m_width * y + x);
					bitmap += 4;
				}
			}
		}
		else {
			std::cout << "Should't be happened" << std::endl;
		}
	}
	else if(color_mode == PSD_MODE_CMYK) {
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				double C	= 1.0 - *(m_chBufs[0] + m_width * y + x) / 255.0;	// C
				double M	= 1.0 - *(m_chBufs[1] + m_width * y + x) / 255.0;	// M
				double Y	= 1.0 - *(m_chBufs[2] + m_width * y + x) / 255.0;	// Y
				double K	= 1.0 - *(m_chBufs[3] + m_width * y + x) / 255.0;	// K

				bitmap[0]	= (unsigned char)((1.0 - (C * (1.0 - K) + K)) * 255.0);
				bitmap[1]	= (unsigned char)((1.0 - (M * (1.0 - K) + K)) * 255.0);
				bitmap[2]	= (unsigned char)((1.0 - (Y * (1.0 - K) + K)) * 255.0);
				bitmap	+= 3;
			}
		}
	}

	cleanup();

	return true;
}

void CFormatPsd::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}

bool CFormatPsd::skipNextBlock() {
	uint32 size;
	if(sizeof(uint32) != fread(&size, 1, sizeof(uint32), m_file)) {
		return false;
	}
	size	= read_uint32((uint8*)&size);
//	std::cout << size << " bytes skipped" << std::endl;
	fseek(m_file, size, SEEK_CUR);

	return true;
}

void CFormatPsd::decompressLine(const uint8* src, uint32 lineLength, uint8* dest) {
	uint16 bytes_read	= 0;
	while(bytes_read < lineLength) {
		signed char byte	= src[bytes_read];
		bytes_read++;

		if(byte == -128) {
			continue;
		}
		else if(byte > -1) {
			int count	= byte + 1;

			// copy next count bytes
			for(int i = 0; i < count; i++) {
				*dest	= src[bytes_read];
				dest++;
				bytes_read++;
			}
		}
		else {
			int count	= -byte + 1;

			// copy next byte count times
			uint8 next_byte	= src[bytes_read];
			bytes_read++;
			for(int i = 0; i < count; i++) {
				*dest	= next_byte;
				dest++;
			}
		}
	}
}

void CFormatPsd::cleanup(){
	delete[] m_buffer;
	delete[] m_linesLengths;
	for(int i = 0; i < MAX_CHANNELS; i++) {
		delete[] m_chBufs[i];
	}
	if(m_file != 0) {
		fclose(m_file);
	}
}
