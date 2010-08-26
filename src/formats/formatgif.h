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

	virtual bool Load(const char* filename, int subImage = 0);
	virtual void FreeMemory();

private:
	int readUntilImage(GifFileType* file);
};

#endif // FORMATGIF_H
