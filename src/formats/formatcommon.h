/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATCOMMON_H
#define FORMATCOMMON_H

#include <Imlib2.h>
#include "format.h"

class CFormatCommon : public CFormat {
public:
	CFormatCommon();
	virtual ~CFormatCommon();

	bool Load(const char* filename, int sub_image = 0);
	void FreeMemory();

protected:
private:
	Imlib_Image m_image;
};

#endif // FORMATCOMMON_H
