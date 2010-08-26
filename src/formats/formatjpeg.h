/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATJPEG_H
#define FORMATJPEG_H

#include "format.h"

class CFormatJpeg : public CFormat {
public:
	CFormatJpeg(Callback callback);
	virtual ~CFormatJpeg();

	virtual bool Load(const char* filename, int subImage = 0);
	virtual void FreeMemory();

private:
};

#endif // FORMATJPEG_H
