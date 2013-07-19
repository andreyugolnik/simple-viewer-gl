 /**********************************************\
 *
 *  Andrey A. Ugolnik
 *  Tiny Orange
 *  http://www.tinyorange.com
 *  andrey@ugolnik.info
 *
 *  created: 18.07.2013
 *
 \**********************************************/

#ifndef RLE_H_64B157C817A970
#define RLE_H_64B157C817A970

class cRLE  
{
public:
    cRLE();
    virtual ~cRLE();

    // returns 0 if error occured
    // or length in bytes of encoded data in rle
    unsigned encode(const unsigned char* in, unsigned in_size, unsigned char* const rle, unsigned rle_size);

    // returns 0 if error occured
    // or length in unsigned int of encoded data in rle
    unsigned encodeBy4(const unsigned* in, unsigned in_size, unsigned* const rle, unsigned rle_size);

    // returns 0 if error occured
    // or length in bytes of decoded data in out
    unsigned decode(const unsigned char* rle, unsigned rle_size, unsigned char* const out, unsigned out_size);

    // returns 0 if error occured
    // or length in unsigned int of decoded data in out
    unsigned decodeBy4(const unsigned* rle, unsigned rle_size, unsigned* const out, unsigned out_size);

    // returns desired buffer size if error occured
    unsigned getDesiredSize() const { return m_desired_size; }

private:
    unsigned m_desired_size;
};

#endif /* end of include guard: RLE_H_64B157C817A970 */

