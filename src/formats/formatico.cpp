/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatico.h"
#include <string.h>
#include <math.h>

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

	IcoDirentry* images	= new IcoDirentry[header.count];
	if(header.count != fread(images, sizeof(IcoDirentry), header.count, m_file)) {
		delete[] images;
		fclose(m_file);
		return false;
	}

	sub_image	= std::min(sub_image, header.count - 1);
	IcoDirentry* image	= &images[sub_image];
//	std::cout << std::endl;
//	std::cout << "--- IcoDirentry ---" << std::endl;
//	std::cout << "width: " << (int)image->width << "." << std::endl;
//	std::cout << "height: " << (int)image->height << "." << std::endl;
//	std::cout << "colors: " << (int)image->colors << "." << std::endl;
//	std::cout << "planes: " << (int)image->planes << "." << std::endl;
//	std::cout << "bits: " << (int)image->bits << "." << std::endl;
//	std::cout << "size: " << (int)image->size << "." << std::endl;
//	std::cout << "offset: " << (int)image->offset << "." << std::endl;

	fseek(m_file, image->offset, SEEK_SET);
	uint8* p	= new uint8[image->size];
	if(1 != fread(p, image->size, 1, m_file)) {
		delete[] p;
		delete[] images;
		fclose(m_file);
		return false;
	}

	IcoBmpInfoHeader* imgHeader	= (IcoBmpInfoHeader*)p;
	m_width		= imgHeader->width;
	m_height	= imgHeader->height / 2;	// xor mask + and mask
	m_pitch		= m_width * 4;
	m_bpp		= 32;
	m_bppImage	= imgHeader->bits;
	m_sizeMem	= m_pitch * m_height;
	m_bitmap	= new uint8[m_sizeMem];

//	std::cout << std::endl;
//	std::cout << "--- IcoBmpInfoHeader ---" << std::endl;
//	std::cout << "size: " << (int)imgHeader->size << "." << std::endl;
//	std::cout << "width: " << (int)imgHeader->width << "." << std::endl;
//	std::cout << "height: " << (int)imgHeader->height << "." << std::endl;
//	std::cout << "planes: " << (int)imgHeader->planes << "." << std::endl;
//	std::cout << "bits: " << (int)imgHeader->bits << "." << std::endl;
//	std::cout << "imagesize: " << (int)imgHeader->imagesize << "." << std::endl;

	int pitch	= calcIcoPitch();
	int colors	= image->colors == 0 ? (1 << m_bppImage) : image->colors;
	uint32* palette	= (uint32*)&p[imgHeader->size];
	uint8* xorMask	= &p[imgHeader->size + colors * 4];
	uint8* andMask	= &p[imgHeader->size + colors * 4 + m_height * pitch];

	switch(m_bppImage) {
	case 1:
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				uint32 color	= palette[getBit(xorMask, y * m_width + x)];

				int idx	= (m_height - y - 1) * m_pitch + x * 4;

				m_bitmap[idx + 0]	= ((uint8*)(&color))[2];
				m_bitmap[idx + 1]	= ((uint8*)(&color))[1];
				m_bitmap[idx + 2]	= ((uint8*)(&color))[0];

				if(getBit(andMask, y * m_width + x)) {
					m_bitmap[idx + 3]	= 0;
				}
				else {
					m_bitmap[idx + 3]	= 255;
				}

				int percent	= (int)(100.0f * m_height * m_width / (y * m_width + x));
				progress(percent);
			}
		}
		break;

	case 4:
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				uint32 color	= palette[getNibble(xorMask, y * m_width + x)];

				int idx	= (m_height - y - 1) * m_pitch + x * 4;

				m_bitmap[idx + 0]	= ((uint8*)(&color))[2];
				m_bitmap[idx + 1]	= ((uint8*)(&color))[1];
				m_bitmap[idx + 2]	= ((uint8*)(&color))[0];

				if(getBit(andMask, y * m_width + x)) {
					m_bitmap[idx + 3]	= 0;
				}
				else {
					m_bitmap[idx + 3]	= 255;
				}

				int percent	= (int)(100.0f * m_height * m_width / (y * m_width + x));
				progress(percent);
			}
		}
		break;

	case 8:
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				uint32 color	= palette[getByte(xorMask, y * m_width + x)];

				int idx	= (m_height - y - 1) * m_pitch + x * 4;

				m_bitmap[idx + 0]	= ((uint8*)(&color))[2];
				m_bitmap[idx + 1]	= ((uint8*)(&color))[1];
				m_bitmap[idx + 2]	= ((uint8*)(&color))[0];

				if(getBit(andMask, y * m_width + x)) {
					m_bitmap[idx + 3]	= 0;
				}
				else {
					m_bitmap[idx + 3]	= 255;
				}

				int percent	= (int)(100.0f * m_height * m_width / (y * m_width + x));
				progress(percent);
			}
		}
		break;

	default:
		{
			int bpp		= m_bppImage / 8;
			for(int y = 0; y < m_height; y++) {

				uint8* row	= xorMask + pitch * y;

				for(int x = 0; x < m_width; x++) {
					int idx	= (m_height - y - 1) * m_pitch + x * 4;

					m_bitmap[idx + 0]	= row[2];
					m_bitmap[idx + 1]	= row[1];
					m_bitmap[idx + 2]	= row[0];

					if(m_bppImage < 32) {
						if(getBit(andMask, y * m_width + x)) {
							m_bitmap[idx + 3]	= 0;
						}
						else {
							m_bitmap[idx + 3]	= 255;
						}
					}
					else {
						m_bitmap[idx + 3]	= row[3];
					}

					row	+= bpp;

					int percent	= (int)(100.0f * m_height * m_width / (y * m_width + x));
					progress(percent);
				}
			}
		}
		break;
	}

	delete[] p;
	delete[] images;
	fclose(m_file);
	return true;
}

void CFormatIco::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}

int CFormatIco::calcIcoPitch() {
	switch(m_bppImage) {
	case 1:
		if((m_width % 32) == 0)
			return m_width / 8;
		return 4 * (m_width / 32 + 1);

	case 4:
		if((m_width % 8) == 0)
			return m_width / 2;
		return 4 * (m_width / 8 + 1);

	case 8:
		if((m_width % 4) == 0)
			return m_width;
		return 4 * (m_width / 4 + 1);

	case 24:
		if(((m_width * 3) % 4) == 0)
			return m_width * 3;
		return 4 * (m_width * 3 / 4 + 1);

	case 32:
		return m_width * 4;

	default:
		std::cout << "Invalid bits count: " << m_bppImage << std::endl;
		return m_width * (m_bppImage / 8);
	}
}

int CFormatIco::getBit(const uint8* data, int bit) {
	// width per line in multiples of 32 bits
	int width32	= (m_width % 32 == 0 ? m_width / 32 : m_width / 32 + 1);
	int line	= bit / m_width;
	int offset	= bit % m_width;

	int result	= (data[line * width32 * 4 + offset / 8] & (1 << (7 - (offset % 8))));

	return (result ? 1 : 0);
}

int CFormatIco::getNibble(const uint8* data, int nibble) {
	// width per line in multiples of 32 bits
	int width32	= (m_width % 8 == 0 ? m_width / 8 : m_width / 8 + 1);
	int line	= nibble / m_width;
	int offset	= nibble % m_width;

	int result	= (data[line * width32 * 4 + offset / 2] & (0x0F << (4 * (1 - offset % 2))));

	if(offset % 2 == 0) {
		result	= result >> 4;
	}

	return result;
}

int CFormatIco::getByte(const uint8* data, int byte) {
	// width per line in multiples of 32 bits
	int width32	= (m_width % 4 == 0 ? m_width / 4 : m_width / 4 + 1);
	int line	= byte / m_width;
	int offset	= byte % m_width;

	return data[line * width32 * 4 + offset];
}
