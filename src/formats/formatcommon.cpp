/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatcommon.h"
#include <iostream>
#include <string.h>

CFormatCommon* g_this = 0;

CFormatCommon::CFormatCommon(Callback callback)
    : CFormat(callback)
    , m_image(0)
{
	g_this = this;
	imlib_context_set_progress_function(callbackProgress);
	imlib_context_set_progress_granularity(10);	// update progress each 10%
}

CFormatCommon::~CFormatCommon()
{
}

bool CFormatCommon::Load(const char* filename, int subImage)
{
	if(openFile(filename) == false)
	{
		return false;
	}
	reset();

	// try to load image from disk
	Imlib_Load_Error error_return;
	m_image	= imlib_load_image_with_error_return(filename, &error_return);
	if(m_image == 0)
	{
		std::cout << ": error loading file '" << filename << "' (" << error_return << ")" << std::endl;
		reset();
		return false;
	}

	imlib_context_set_image(m_image);

	m_width		= imlib_image_get_width();
	m_height	= imlib_image_get_height();
	m_pitch		= 4 * m_width;
	m_bpp		= 32;	// Imlib2 always has 32-bit buffer, but sometimes alpha not used
	m_bppImage	= (imlib_image_has_alpha() == 1 ? 32 : 24);

	m_bitmap.resize(m_pitch * m_height);
	memcpy(&m_bitmap[0], imlib_image_get_data_for_reading_only(), m_bitmap.size());

	m_format	= GL_BGRA;

	return true;
}

void CFormatCommon::FreeMemory()
{
    if(m_image != 0)
    {
        imlib_free_image();
        m_image = 0;
    }

    CFormat::FreeMemory();
}

int CFormatCommon::callbackProgress(void* p, char percent, int a, int b, int c, int d)
{
	g_this->progress(percent);
	return 1;
}

