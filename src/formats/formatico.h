/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATICO_H
#define FORMATICO_H

#include "format.h"

class CFormatIco : public CFormat {
public:
	CFormatIco(Callback callback);
	virtual ~CFormatIco();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

private:
};

#endif // FORMATICO_H
