/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATGIF_H
#define FORMATGIF_H

#include "format.h"
#include <gif_lib.h>

class CFormatGif : public CFormat {
public:
	CFormatGif(Callback callback);
	virtual ~CFormatGif();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

private:
	int ReadUntilImage(GifFileType *GifFile);
};

#endif // FORMATGIF_H
