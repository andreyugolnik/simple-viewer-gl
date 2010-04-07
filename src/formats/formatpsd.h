/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATPSD_H
#define FORMATPSD_H

#include "format.h"

namespace FORMAT_PSD {
#pragma pack(push, 1)
	typedef struct PSD_HEADER {
		uint8	signature[4];  /* file ID, always "8BPS" */
		uint16	version;       /* version number, always 1 */
		uint8	resetved[6];
		uint16	channels;      /* number of color channels (1-24) */
		uint32	rows;          /* height of image in pixels (1-30000) */
		uint32	columns;       /* width of image in pixels (1-30000) */
		uint16	depth;         /* number of bits per channel (1, 8, and 16) */
		uint16	color_mode;    /* color mode as defined below */
	} PsdHeader;
#pragma pack(pop)

	typedef enum COLOR_MODE {
		PSD_MODE_MONO			= 0,
		PSD_MODE_GRAYSCALE		= 1,
		PSD_MODE_INDEXED		= 2,
		PSD_MODE_RGB			= 3,
		PSD_MODE_CMYK			= 4,
		PSD_MODE_MULTICHANNEL	= 7,
		PSD_MODE_DUOTONE		= 8,
		PSD_MODE_LAB			= 9,
	} ColorMode;

};

class CFormatPsd : public CFormat {
public:
	CFormatPsd(Callback callback);
	virtual ~CFormatPsd();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

private:
	uint8* m_buffer;
	uint16* m_linesLengths;
	#define MAX_CHANNELS	24
	uint8* m_chBufs[MAX_CHANNELS];

private:
	bool skipNextBlock();
	void decompressLine(const uint8* src, uint32 line_length, uint8* dest);
	void cleanup();
};

#endif // FORMATPSD_H
