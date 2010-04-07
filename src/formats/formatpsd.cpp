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

CFormatPsd::CFormatPsd(Callback callback) : CFormat(callback) {
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
		fclose(m_file);
		return false;
	}

	if(header.signature[0] != '8' || header.signature[1] != 'B' || header.signature[2] != 'P' || header.signature[3] != 'S') {
		std::cout << "Not valid PSD file" << std::endl;
		fclose(m_file);
		return false;
	}

	int color_mode	= read_uint16((uint8*)&header.color_mode);
	if(color_mode != PSD_MODE_RGB) {
		std::cout << "Unsupported color mode: " << color_mode << std::endl;
		fclose(m_file);
		return false;
	}

	int depth		= read_uint16((uint8*)&header.depth);
	if(depth != 8) {
		std::cout << "Unsupported depth: " << depth << std::endl;
		fclose(m_file);
		return false;
	}

	int channels	= read_uint16((uint8*)&header.channels);
	if(channels != 3 && channels != 4) {
		std::cout << "Unsupported cannels count: " << channels << std::endl;
		fclose(m_file);
		return false;
	}

	// skip Color Mode Data Block
	if(false == skipNextBlock()) {
		std::cout << "Can't read Color Mode Data Block" << std::endl;
		fclose(m_file);
		return false;
	}

	// skip Image Resources Block
	if(false == skipNextBlock()) {
		std::cout << "Can't read Image Resources Block" << std::endl;
		fclose(m_file);
		return false;
	}

	// Layer and Mask Information Block
	if(false == skipNextBlock()) {
		std::cout << "Can't read Layer and Mask Information Block" << std::endl;
		fclose(m_file);
		return false;
	}

	// Image Data Block
	uint16 compression;
	if(sizeof(uint16) != fread(&compression, 1, sizeof(uint16), m_file)) {
		std::cout << "Can't read compression info" << std::endl;
		fclose(m_file);
		return false;
	}
	compression	= read_uint16((uint8*)&compression);

	m_width		= read_uint32((uint8*)&header.columns);
	m_height	= read_uint32((uint8*)&header.rows);
	m_bpp		= depth * channels;
	m_bppImage	= m_bpp;

	// this will be needed for RLE decompression
	uint16* lines_lengths	= new uint16[channels * m_height];

	if(m_height * channels != fread(lines_lengths, 2, m_height * channels, m_file)) {
		std::cout << "Can't read length of lines" << std::endl;
		delete[] lines_lengths;
		fclose(m_file);
		return false;
	}

	// convert from different endianness
	for(int i = 0; i < m_height * channels; i++) {
		lines_lengths[i]	= read_uint16((uint8*)&lines_lengths[i]);
	}

	// we need buffer that can contain one channel data of one
	// row in RLE compressed format. 2*width should be enough
	uint8* buffer	= new uint8[m_width * 2];

	// create separate buffers for each channel
	uint8* ch_bufs[4]	= { 0, 0, 0, 0 };
	for(int i = 0; i < channels; i++) {
		ch_bufs[i]	= new uint8[m_width * m_height];
	}


	int curr_row	= 0;
	int curr_ch		= 0;
	int pos			= 0;

	bool done	= false;
	do {
		int line_length	= m_width;
		if(compression == 1) {
			line_length	= lines_lengths[curr_ch * m_height + curr_row];
		}

		fread(buffer, 1, line_length, m_file);

//		int percent	= (int)(100.0f * cinfo.output_scanline / cinfo.output_height);
//		progress(percent);

		if(compression == 1) {
			decompressLine(buffer, line_length, ch_bufs[curr_ch] + pos);
		}
		else {
			memcpy(ch_bufs[curr_ch] + pos, buffer, m_width);
		}

		pos	+= m_width;
		curr_row++;

		if(curr_row >= m_height) {
			curr_ch++;
			curr_row	= 0;
			pos			= 0;
			if(curr_ch >= channels) {
				done	= true;
			}
		}
	} while(done == false);





	// convert or copy channel buffers to BGR / BGRA
	m_pitch		= m_width * channels;
	m_bitmap	= new unsigned char[m_pitch * m_height];
	unsigned char* bitmap	= m_bitmap;

	if(color_mode == PSD_MODE_RGB && channels == 3) {
		for(int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				bitmap[0]	= *(ch_bufs[2] + m_width * y + x);
				bitmap[1]	= *(ch_bufs[1] + m_width * y + x);
				bitmap[2]	= *(ch_bufs[0] + m_width * y + x);
				bitmap	+= 3;
			}
		}
	}
	else if(color_mode == PSD_MODE_RGB && channels == 4) {
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				bitmap[0]	= *(ch_bufs[2] + m_width * y + x);
				bitmap[1]	= *(ch_bufs[1] + m_width * y + x);
				bitmap[2]	= *(ch_bufs[0] + m_width * y + x);
				bitmap[3]	= *(ch_bufs[3] + m_width * y + x);
				bitmap += 4;
			}
		}
	}
	else if(color_mode == PSD_MODE_CMYK) {
		// unfortunately, this doesn't seem to work correctly...
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				double C	= 1.0 - (double)*(ch_bufs[0] + m_width * y + x) / 255.0;	// C
				double M	= 1.0 - (double)*(ch_bufs[1] + m_width * y + x) / 255.0;	// M
				double Y	= 1.0 - (double)*(ch_bufs[2] + m_width * y + x) / 255.0;	// Y
				double K	= 1.0 - (double)*(ch_bufs[3] + m_width * y + x) / 255.0;	// K

				bitmap[0]	= (unsigned char)((1.0 - (C * (1.0 - K) + K)) * 255.0);
				bitmap[1]	= (unsigned char)((1.0 - (M * (1.0 - K) + K)) * 255.0);
				bitmap[2]	= (unsigned char)((1.0 - (Y * (1.0 - K) + K)) * 255.0);
				bitmap	+= 3;
			}
		}
	}

	for(int i = 0; i < channels; i++) {
		delete[] ch_bufs[i];
	}

	delete[] lines_lengths;
	delete[] buffer;

	fclose(m_file);

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
	std::cout << size << " bytes skipped" << std::endl;
	fseek(m_file, size, SEEK_CUR);

	return true;
}

void CFormatPsd::decompressLine(const uint8* src, uint32 line_length, uint8* dest) {
	uint16 bytes_read	= 0;
	while(bytes_read < line_length) {
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
