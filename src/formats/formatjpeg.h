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
	CFormatJpeg();
	virtual ~CFormatJpeg();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

private:
};

#endif // FORMATJPEG_H
