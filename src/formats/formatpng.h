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

	virtual bool Load(const char* filename, int subImage = 0);
	virtual void FreeMemory();

private:
};

#endif // FORMATPNG_H
