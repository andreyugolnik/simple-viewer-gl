/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "format.h"

CFormat::CFormat(Callback callback)
    : m_callback(callback)
    , m_percent(-1)
    , m_file(0)
    , m_bitmap(0)
    , m_format(GL_RGB)
    , m_width(0)
    , m_height(0)
    , m_pitch(0)
    , m_bpp(0)
    , m_bppImage(0)
    , m_size(-1) // -1 mean that file can't be opened
    , m_sizeMem(0)
    , m_subImage(0)
    , m_subCount(0)
{
}

CFormat::~CFormat()
{
}

void CFormat::FreeMemory()
{
    delete[] m_bitmap;
    m_bitmap = 0;
}

bool CFormat::openFile(const char* path)
{
	m_file = fopen(path, "rb");
	if(m_file == 0)
	{
		std::cout << "Can't open \"" << path << "\"." << std::endl;
		return false;
	}

	fseek(m_file, 0, SEEK_END);
	m_size = ftell(m_file);
	fseek(m_file, 0, SEEK_SET);

	return true;
}

void CFormat::progress(int percent)
{
	if(m_callback != 0)
	{
		if(m_percent != percent)
		{
			m_percent	= percent;
			m_callback(percent);
		}
	}
}

void CFormat::reset()
{
	if(m_file != 0)
	{
		fclose(m_file);
		m_file = 0;
	}

	delete[] m_bitmap;
	m_bitmap	= 0;
	m_format	= GL_RGB;
	m_width		= 0;
	m_height	= 0;
	m_pitch		= 0;
	m_bpp		= 0;
	m_bppImage	= 0;
	m_size		= -1;
	m_sizeMem	= 0;
	m_subImage	= 0;
	m_subCount	= 0;
	m_info.clear();
}

uint16_t CFormat::read_uint16(uint8_t* p)
{
    return (p[0] << 8) | p[1];
}

uint32_t CFormat::read_uint32(uint8_t* p)
{
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

void CFormat::swap_long(uint8_t* bp, uint32_t n)
{
    uint8_t* ep = bp + n;

    while(bp < ep)
    {
        uint8_t c = bp[3];
        bp[3] = bp[0];
        bp[0] = c;
        c = bp[2];
        bp[2] = bp[1];
        bp[1] = c;
        bp += 4;
    }
}

