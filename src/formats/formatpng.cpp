/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatpng.h"
#include <png.h>
#include <string.h>
#include <iostream>

CFormatPng::CFormatPng(Callback callback) : CFormat(callback) {
}

CFormatPng::~CFormatPng() {
	FreeMemory();
}

bool CFormatPng::Load(const char* filename, int subImage) {
	if(openFile(filename) == false) {
		return false;
	}

	png_byte header[8];	// 8 is the maximum size that can be checked
	size_t size	= fread(header, 1, 8, m_file);
	if(size != 8 || png_sig_cmp(header, 0, 8) != 0) {
		std::cout << "File " << filename << " is not recognized as a PNG file" << std::endl;
		fclose(m_file);
		return false;
	}

	// initialize stuff
	png_structp png	= png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png == 0) {
		std::cout << "png_create_read_struct failed" << std::endl;
		fclose(m_file);
		return false;
	}

	png_infop info	= png_create_info_struct(png);
	if(info == 0) {
		std::cout << "png_create_info_struct failed" << std::endl;
		fclose(m_file);
		return false;
	}

	if(setjmp(png_jmpbuf(png)) != 0) {
		std::cout << "Error during init_io" << std::endl;
		fclose(m_file);
		return false;
	}

	png_init_io(png, m_file);
	png_set_sig_bytes(png, 8);

	png_read_info(png, info);

	// get real bits per pixel
	m_bppImage	= info->pixel_depth;

	if(info->color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png);
	}

#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
	if(info->color_type == PNG_COLOR_TYPE_GRAY && info->bit_depth < 8) {
		// depreceted in libPNG-1.4.2
        png_set_gray_1_2_4_to_8(png);
	}
#endif

    if(png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
	if(info->bit_depth == 16) {
		png_set_strip_16(png);
	}
	if(info->color_type == PNG_COLOR_TYPE_GRAY || info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png);
	}

//	int number_of_passes	= png_set_interlace_handling(png);
	png_read_update_info(png, info);

	m_width		= info->width;
	m_height	= info->height;
	m_pitch		= info->rowbytes;
	m_bpp		= info->pixel_depth;

	// read file
	if(setjmp(png_jmpbuf(png)) != 0) {
		std::cout << "Error during read_image" << std::endl;
		fclose(m_file);
		return false;
	}

	// create buffer and read data
	png_bytep* row_pointers	= new png_bytep[m_height];
	for(int y = 0; y < m_height; y++) {
		row_pointers[y]	= new png_byte[info->rowbytes];
	}
	png_read_image(png, row_pointers);

	// create BGRA buffer and decode image data
	m_sizeMem	= m_pitch * m_height;
	m_bitmap	= new unsigned char[m_sizeMem];

	if(info->color_type == PNG_COLOR_TYPE_RGB) {
		for(int y = 0; y < m_height; y++) {
			int dst	= y * m_pitch;
			for(int x = 0; x < m_width; x++) {
				int dx	= x * 3;
				m_bitmap[dst + dx + 0]	= *(row_pointers[y] + dx + 0);
				m_bitmap[dst + dx + 1]	= *(row_pointers[y] + dx + 1);
				m_bitmap[dst + dx + 2]	= *(row_pointers[y] + dx + 2);
			}

			int percent	= (int)(100.0f * y / m_height);
			progress(percent);

			delete[] row_pointers[y];
		}
	}
	else if(info->color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
		for(int y = 0; y < m_height; y++) {
			int dst	= y * m_pitch;
			for(int x = 0; x < m_width; x++) {
				int dx	= x * 4;
				m_bitmap[dst + dx + 0]	= *(row_pointers[y] + dx + 0);
				m_bitmap[dst + dx + 1]	= *(row_pointers[y] + dx + 1);
				m_bitmap[dst + dx + 2]	= *(row_pointers[y] + dx + 2);
				m_bitmap[dst + dx + 3]	= *(row_pointers[y] + dx + 3);
			}

			int percent	= (int)(100.0f * y / m_height);
			progress(percent);

			delete[] row_pointers[y];
		}
	}
	else {
		for(int y = 0; y < m_height; y++) {
			delete[] row_pointers[y];
		}
		std::cout << "Should't be happened" << std::endl;
	}

	delete[] row_pointers;

	png_destroy_read_struct(&png, &info, NULL);

	fclose(m_file);

	return true;
}

void CFormatPng::FreeMemory() {
	delete[] m_bitmap;
	m_bitmap	= 0;
}

