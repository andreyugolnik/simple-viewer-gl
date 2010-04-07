/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATPNG_H
#define FORMATPNG_H

#include "format.h"

class CFormatPng : public CFormat {
public:
	CFormatPng(Callback callback);
	virtual ~CFormatPng();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

private:
};

#endif // FORMATPNG_H
