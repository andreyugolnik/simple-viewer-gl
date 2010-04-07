/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "imageloader.h"
#include "formats/formatcommon.h"
#include "formats/formatjpeg.h"
#include "formats/formatpsd.h"
#include <iostream>
#include <algorithm>

CImageLoader::CImageLoader(Callback callback) : m_angle(ANGLE_0), m_callback(callback) {
}

CImageLoader::~CImageLoader() {
}

bool CImageLoader::LoadImage(const char* path, int sub_image) {
	if(path != 0) {
		if(m_path.empty() == false && m_path == path) {
			return true;	// image already loaded
		}

		m_angle		= ANGLE_0;
		m_path		= path;

		int format	= getFormat();
		if(format == FORMAT_JPEG) {
			m_image.reset(new CFormatJpeg(m_callback));
		}
		else if(format == FORMAT_PSD) {
			m_image.reset(new CFormatPsd(m_callback));
		}
		else {
			m_image.reset(new CFormatCommon(m_callback));
		}

		return m_image->Load(path);
	}

	return false;
}

unsigned char* CImageLoader::GetBitmap() const {
	if(m_image.get() != 0) {
		return (unsigned char*)m_image->m_bitmap;
	}
	return 0;
}

void CImageLoader::FreeMemory() {
	if(m_image.get() != 0) {
		m_image->FreeMemory();
	}
}

int CImageLoader::GetWidth() const {
	if(m_image.get() != 0) {
		if(m_angle == ANGLE_90 || m_angle == ANGLE_270) {
			return m_image->m_height;
		}
		return m_image->m_width;
	}
	return 0;
}

int CImageLoader::GetHeight() const {
	if(m_image.get() != 0) {
		if(m_angle == ANGLE_90 || m_angle == ANGLE_270) {
			return m_image->m_width;
		}
		return m_image->m_height;
	}
	return 0;
}

int CImageLoader::GetBpp() const {
	if(m_image.get() != 0) {
		return m_image->m_bpp;
	}
	return 0;
}

int CImageLoader::GetImageBpp() const {
	if(m_image.get() != 0) {
		return m_image->m_bppImage;
	}
	return 0;
}

// file size on disk
long CImageLoader::GetSize() const {
	if(m_image.get() != 0) {
		return m_image->m_size;
	}
	return 0;
}

//int CImageLoader::GetSub() const {
////	if(m_image.get() != 0) {
////		return m_image->m_subImage;
////	}
//	return 0;
//}
//
//int CImageLoader::GetSubCount() const {
////	if(m_image.get() != 0) {
////		return m_image->m_subCount;
////	}
//	return 0;
//}

int CImageLoader::getFormat() {
	std::string s(m_path);

	// skip file without extension
	size_t pos	= s.find_last_of('.');
	if(std::string::npos == pos) {
		return false;
	}

	// skip non image file (detect by extension)
	std::transform(s.begin(), s.end(), s.begin(), tolower);

	FORMAT format[]	= {
		{ ".jpeg", FORMAT_JPEG },
		{ ".jpg",  FORMAT_JPEG },
		{ ".psd",  FORMAT_PSD  },
	};

	for(size_t i = 0; i < sizeof(format) / sizeof(FORMAT); i++) {
		if(s.substr(pos) == format[i].ext) {
			return format[i].format;
		}
	}

	return FORMAT_COMMON;
}
