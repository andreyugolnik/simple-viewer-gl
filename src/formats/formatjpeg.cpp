/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatjpeg.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"
#include "../common/helpers.h"
#include "../cms/cms.h"

#include <cstring>
#include <jpeglib.h>
#include <setjmp.h>

namespace
{

    struct sErrorMgr
    {
        struct jpeg_error_mgr pub;  /* "public" fields */
        jmp_buf setjmp_buffer;  /* for return to caller */
    };

    void ErrorExit(j_common_ptr cinfo)
    {
        // cinfo->err really points to a sErrorMgr struct, so coerce pointer
        auto errMgr = reinterpret_cast<sErrorMgr*>(cinfo->err);

        // Always display the message.
        // We could postpone this until after returning, if we chose.
        (*cinfo->err->output_message)(cinfo);

        // Return control to the setjmp point
        longjmp(errMgr->setjmp_buffer, 1);
    }

    // const char* getFormat(unsigned idx)
    // {
        // const char* Formats[] =
        // {
            // "jpeg",           // JCS_UNKNOWN:
            // "jpeg/grayscale", // JCS_GRAYSCALE:
            // "jpeg/rgb",       // JCS_RGB:
            // "jpeg/ycbcr",     // JCS_YCbCr:
            // "jpeg/cmyk",      // JCS_CMYK:
            // "jpeg/ycck",      // JCS_YCCK:
        // };
        // const auto size = helpers::countof(Formats);
        // return Formats[idx < size ? idx : 0];
    // }

    const unsigned int maxMarkerLength = 0xffff;
    // const int JPEG_EXIF = JPEG_APP0 + 1;  // Exif/XMP
    const int JPEG_ICCP = JPEG_APP0 + 2;  // ICC profile

    void setupMarkers(jpeg_decompress_struct* cinfo)
    {
        // jpeg_save_markers(cinfo, JPEG_EXIF, maxMarkerLength);
        jpeg_save_markers(cinfo, JPEG_ICCP, maxMarkerLength);
    }

    void* locateICCProfile(const jpeg_decompress_struct& cinfo, unsigned& iccProfileSize)
    {
        static const char kICCPSignature[] = "ICC_PROFILE";
        static const size_t kICCPSkipLength = 14;  // signature + seq & count

        for (auto m = cinfo.marker_list; m != nullptr; m = m->next)
        {
            if (m->marker == JPEG_ICCP
                && m->data_length > kICCPSkipLength
                && ::memcmp(m->data, kICCPSignature, sizeof(kICCPSignature)) == 0)
            {
                iccProfileSize = m->data_length - kICCPSkipLength;
                return m->data + kICCPSkipLength;
            }
        }

        return nullptr;
    }

}

cFormatJpeg::cFormatJpeg(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatJpeg::~cFormatJpeg()
{
}

bool cFormatJpeg::isSupported(cFile& file, Buffer& buffer) const
{
    struct JFIFHeader
    {
        uint8_t soi[2];          /* 00h  Start of Image Marker     */
        uint8_t app0[2];         /* 02h  Application Use Marker    */
        uint8_t length[2];       /* 04h  Length of APP0 Field      */
        uint8_t identifier[5];   /* 06h  "JFIF" (zero terminated) Id String */
        uint8_t version[2];      /* 07h  JFIF Format Revision      */
        uint8_t units;           /* 09h  Units used for Resolution */
        uint8_t xdensity[2];     /* 0Ah  Horizontal Resolution     */
        uint8_t ydensity[2];     /* 0Ch  Vertical Resolution       */
        uint8_t xthumbnail;      /* 0Eh  Horizontal Pixel Count    */
        uint8_t ythumbnail;
    };

    if (!readBuffer(file, buffer, sizeof(JFIFHeader)))
    {
        return false;
    }

    const auto h = reinterpret_cast<const JFIFHeader*>(buffer.data());
    const uint8_t soi[2] = { 0xFF, 0xD8 };
    // const uint8_t app0[2] = { 0xFF, 0xE0 };
    return (!::memcmp(h->soi, soi, 2) //&& !::memcmp(h->app0, app0, 2)
            && (!::strcmp((const char*)h->identifier, "JFIF")
                || !::strcmp((const char*)h->identifier, "Exif")
                || !::strcmp((const char*)h->identifier, "JFXX")));
}

bool cFormatJpeg::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    // Step 1: allocate and initialize JPEG decompression object

    // This struct contains the JPEG decompression parameters and pointers to
    // working space (which is allocated as needed by the JPEG library).
    jpeg_decompress_struct cinfo;

    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    sErrorMgr jerr;

    // We set up the normal JPEG error routines, then override error_exit.
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = ErrorExit;
    // Establish the setjmp return context for ErrorExit to use.
    if (setjmp(jerr.setjmp_buffer))
    {
        // If we get here, the JPEG code has signaled an error.
        // We need to clean up the JPEG object, close the input file, and return.
        jpeg_destroy_decompress(&cinfo);
        return false;
    }
    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&cinfo);

    // Step 2: specify data source (eg, a file)
    jpeg_stdio_src(&cinfo, (FILE*)file.getHandle());

    // Step 3: read file parameters with jpeg_read_header()
    setupMarkers(&cinfo);
    jpeg_read_header(&cinfo, true);

    unsigned iccProfileSize = 0;
    auto iccProfile = locateICCProfile(cinfo, iccProfileSize);
    m_cms.createTransform(iccProfile, iccProfileSize, cCMS::Pixel::Rgb);

    // m_formatName = getFormat(cinfo.jpeg_color_space);
    m_formatName = m_cms.hasTransform() ? "jpeg/icc" : "jpeg";

    /* Step 4: set parameters for decompression */
    cinfo.out_color_space = JCS_RGB;    // convert to RGB

    /* Step 5: Start decompressor */
    jpeg_start_decompress(&cinfo);

    desc.width = cinfo.output_width;
    desc.height = cinfo.output_height;
    desc.pitch = cinfo.output_width * cinfo.output_components;
    desc.bpp = cinfo.output_components * 8;
    desc.bppImage = cinfo.num_components * 8;
    desc.bitmap.resize(desc.pitch * desc.height);
    desc.format = GL_RGB;

    /* Step 6: while (scan lines remain to be read) */
    /*           jpeg_read_scanlines(...); */

    /* Here we use the library's state variable cinfo.output_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     */
    const auto rowStride = cinfo.output_width * cinfo.output_components;
    auto out = desc.bitmap.data();

    if (m_cms.hasTransform() == false)
    {
        while (cinfo.output_scanline < cinfo.output_height && m_stop == false)
        {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
            jpeg_read_scanlines(&cinfo, &out, 1);
            out += rowStride;

            updateProgress((float)cinfo.output_scanline / cinfo.output_height);
        }
    }
    else
    {
        std::vector<unsigned char> buffer(rowStride);
        auto input = buffer.data();
        while (cinfo.output_scanline < cinfo.output_height && m_stop == false)
        {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
            jpeg_read_scanlines(&cinfo, &input, 1);
            m_cms.doTransform(input, out, cinfo.output_width);
            out += rowStride;

            updateProgress((float)cinfo.output_scanline / cinfo.output_height);
        }

        m_cms.destroyTransform();
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

    /* At this point you may want to check to see whether any corrupt-data
     * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
     */

    return true;
}
