/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATTIFF_H
#define FORMATTIFF_H

#include "format.h"

class CFormatTiff : public CFormat {
public:
	CFormatTiff(Callback callback);
	virtual ~CFormatTiff();

	virtual bool Load(const char* filename, int subImage = 0);
	virtual void FreeMemory();

private:
};

#endif // FORMATTIFF_H
