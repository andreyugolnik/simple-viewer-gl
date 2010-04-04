/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageloader.h"
//#include "imagejpeg.h"
//#include "imagepng.h"
#include <iostream>
#include <fstream>
#include <algorithm>

CImageLoader::CImageLoader() : m_image(0), m_angle(ANGLE_0) {
}

CImageLoader::~CImageLoader() {
	if(m_image != 0) {
		imlib_free_image();
	}
}

bool CImageLoader::LoadImage(const char* path, int sub_image) {
	// image already loaded
	if(m_path.empty() == false && path != 0 && m_path == path) {
		return true;
	}

	// reset image properties
	if(m_image != 0) {
		imlib_free_image();
	}

	m_angle		= ANGLE_0;
	m_path		= path;
	m_width		= 0;
	m_height	= 0;
	m_bpp		= 0;
	m_size		= 0;
	m_data		= 0;
	m_alpha		= false;
	m_format	= "";

	if(path != 0) {
		// get file size
		std::ifstream f(path, std::ios::binary);
		if(f.good() != 0) {
			f.seekg(0, std::ios::end);
			m_size	= f.tellg();
			f.close();
		}

		// try to load image from disk
		Imlib_Load_Error error_return;
		m_image	= imlib_load_image_with_error_return(path, &error_return);
		if(m_image == 0) {
			std::cout << "Error loading file '" << m_path << "': " << error_return << std::endl;
			return false;
		}

		imlib_context_set_image(m_image);

		m_width		= imlib_image_get_width();
		m_height	= imlib_image_get_height();
		m_bpp		= (imlib_image_has_alpha() == 1 ? 32 : 24);
		m_data		= imlib_image_get_data_for_reading_only();
		m_alpha		= (imlib_image_has_alpha() == 1);
		m_format	= imlib_image_format();

		return true;
	}

	return false;
}

unsigned char* CImageLoader::GetBitmap() const {
	if(m_image != 0) {
		return (unsigned char*)m_data;
	}
	return 0;
}

int CImageLoader::GetWidth() const {
	if(m_image != 0) {
		if(m_angle == ANGLE_90 || m_angle == ANGLE_270) {
			return m_height;
		}
		return m_width;
	}
	return 0;
}

int CImageLoader::GetHeight() const {
	if(m_image != 0) {
		if(m_angle == ANGLE_90 || m_angle == ANGLE_270) {
			return m_width;
		}
		return m_height;
	}
	return 0;
}

int CImageLoader::GetBpp() const {
	if(m_image != 0) {
		return m_bpp;
	}
	return 0;
}

bool CImageLoader::HasAlpha() const {
	if(m_image != 0) {
		return m_alpha;
	}
	return false;
}

// file size on disk
long CImageLoader::GetSize() const {
	if(m_image != 0) {
		return m_size;
	}
	return 0;
}

int CImageLoader::GetSub() const {
//	if(m_image.get() != 0) {
//		return m_image->m_subImage;
//	}
	return 0;
}

int CImageLoader::GetSubCount() const {
//	if(m_image.get() != 0) {
//		return m_image->m_subCount;
//	}
	return 0;
}
