 /**********************************************
 *
 *  Andrey A. Ugolnik
 *  'WE' Group
 *  http://www.ugolnik.info
 *  andrey@ugolnik.info
 *
 *  created: 04.10.2012
 *  changed: 05.09.2012
 *  version: 0.0.0.23
 *
 ***********************************************/

#ifndef FORMATRAW_H_2D405816DEDEF1
#define FORMATRAW_H_2D405816DEDEF1

#include "format.h"

class cFormatRaw : public CFormat
{
public:
    cFormatRaw(Callback callback, const char* lib, const char* name);
    virtual ~cFormatRaw();

    virtual bool Load(const char* filename, unsigned subImage);
    virtual bool IsValidFormat(const char* name);

private:
    struct sHeader
    {
        unsigned id;
        unsigned w;
        unsigned h;
        unsigned format;
        unsigned data_size;
    };

    bool isValidFormat(const sHeader& header);
};

#endif /* end of include guard: FORMATRAW_H_2D405816DEDEF1 */

