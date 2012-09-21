/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatjpeg.h"
#include <string.h>
#include <jpeglib.h>
#include <setjmp.h>

CFormatJpeg::CFormatJpeg(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

CFormatJpeg::~CFormatJpeg()
{
}

typedef struct my_error_mgr
{
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
} *my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
    // cinfo->err really points to a my_error_mgr struct, so coerce pointer
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    // Always display the message.
    // We could postpone this until after returning, if we chose.
    (*cinfo->err->output_message) (cinfo);

    // Return control to the setjmp point
    longjmp(myerr->setjmp_buffer, 1);
}

bool CFormatJpeg::Load(const char* filename, int subImage)
{
    if(openFile(filename) == false)
    {
        return false;
    }

    // Step 1: allocate and initialize JPEG decompression object

    // This struct contains the JPEG decompression parameters and pointers to
    // working space (which is allocated as needed by the JPEG library).
    struct jpeg_decompress_struct cinfo;

    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    my_error_mgr jerr;

    // We set up the normal JPEG error routines, then override error_exit.
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    // Establish the setjmp return context for my_error_exit to use.
    if(setjmp(jerr.setjmp_buffer))
    {
        // If we get here, the JPEG code has signaled an error.
        // We need to clean up the JPEG object, close the input file, and return.
        jpeg_destroy_decompress(&cinfo);
        reset();
        return false;
    }
    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&cinfo);

    // Step 2: specify data source (eg, a file)

    jpeg_stdio_src(&cinfo, m_file);

    // Step 3: read file parameters with jpeg_read_header()

    jpeg_read_header(&cinfo, TRUE);

    /* Step 4: set parameters for decompression */

    cinfo.out_color_space = JCS_RGB;	// convert to RGB

    /* Step 5: Start decompressor */
    jpeg_start_decompress(&cinfo);

    m_width = cinfo.output_width;
    m_height = cinfo.output_height;
    m_pitch = cinfo.output_width * cinfo.output_components;
    m_bpp = cinfo.output_components * 8;
    m_bppImage = cinfo.num_components * 8;
    m_bitmap.resize(m_pitch * m_height);
    m_format = GL_RGB;

    /* Step 6: while (scan lines remain to be read) */
    /*           jpeg_read_scanlines(...); */

    /* Here we use the library's state variable cinfo.output_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     */
    int row_stride = cinfo.output_width * cinfo.output_components;
    unsigned char* p = &m_bitmap[0];
    while(cinfo.output_scanline < cinfo.output_height)
    {
        /* jpeg_read_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could ask for
         * more than one scanline at a time if that's more convenient.
         */
        jpeg_read_scanlines(&cinfo, &p, 1);
        p += row_stride;

        int percent = (int)(100.0f * cinfo.output_scanline / cinfo.output_height);
        progress(percent);
    }

    /* Step 7: Finish decompression */

    jpeg_finish_decompress(&cinfo);

    /* Step 8: Release JPEG decompression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

    /* After finish_decompress, we can close the input file.
     * Here we postpone it until after no more JPEG errors are possible,
     * so as to simplify the setjmp error logic above.  (Actually, I don't
     * think that jpeg_destroy can do an error exit, but why assume anything...)
     */
    fclose(m_file);

    /* At this point you may want to check to see whether any corrupt-data
     * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
     */

    return true;
}

