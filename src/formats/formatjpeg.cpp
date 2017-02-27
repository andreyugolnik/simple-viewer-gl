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
#include <libexif/exif-data.h>

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
    const unsigned char JPEG_EXIF = JPEG_APP0 + 1;  // Exif/XMP
    const unsigned char JPEG_ICCP = JPEG_APP0 + 2;  // ICC profile

    void setupMarkers(jpeg_decompress_struct* cinfo)
    {
        jpeg_save_markers(cinfo, JPEG_EXIF, maxMarkerLength);
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

    // Remove spaces on the right of the string
    void trimSpaces(char* buf)
    {
        char* s = buf - 1;
        for (; *buf; ++buf)
        {
            if (*buf != ' ')
            {
                s = buf;
            }
        }
        *++s = 0; // nul terminate the string on the first of the final spaces
    }

    void addExifTag(ExifData* d, ExifIfd ifd, ExifTag tag, sBitmapDescription::ExifList& exifList)
    {
        ExifEntry* entry = exif_content_get_entry(d->ifd[ifd], tag);
        if (entry != nullptr)
        {
            // Get the contents of the tag in human-readable form
            char buf[1024];
            exif_entry_get_value(entry, buf, sizeof(buf));

            trimSpaces(buf);
            if (*buf)
            {
                exifList.push_back({ exif_tag_get_title_in_ifd(tag, ifd), buf });
            }
        }
    }

#if defined(DEBUG)
    void printExifTag(const ExifData* d, ExifIfd ifd, ExifTag tag)
    {
        ExifEntry* entry = exif_content_get_entry(d->ifd[ifd], tag);
        if (entry != nullptr)
        {
            // Get the contents of the tag in human-readable form
            char buf[1024];
            exif_entry_get_value(entry, buf, sizeof(buf));

            // Don't bother printing it if it's entirely blank
            trimSpaces(buf);
            if (*buf != 0)
            {
                ::printf("ifd %d , tag 0x%.4x : '%s': '%s'\n", ifd, tag
                         , exif_tag_get_name_in_ifd(tag, ifd), buf);
            }
        }
    }

    void dumpAllExif(const ExifData* ed)
    {
        const ExifTag Tags[] =
        {
            EXIF_TAG_INTEROPERABILITY_INDEX,
            EXIF_TAG_INTEROPERABILITY_VERSION,
            EXIF_TAG_NEW_SUBFILE_TYPE,
            EXIF_TAG_IMAGE_WIDTH,
            EXIF_TAG_IMAGE_LENGTH,
            EXIF_TAG_BITS_PER_SAMPLE,
            EXIF_TAG_COMPRESSION,
            EXIF_TAG_PHOTOMETRIC_INTERPRETATION,
            EXIF_TAG_FILL_ORDER,
            EXIF_TAG_DOCUMENT_NAME,
            EXIF_TAG_IMAGE_DESCRIPTION,
            EXIF_TAG_MAKE,
            EXIF_TAG_MODEL,
            EXIF_TAG_STRIP_OFFSETS,
            EXIF_TAG_ORIENTATION,
            EXIF_TAG_SAMPLES_PER_PIXEL,
            EXIF_TAG_ROWS_PER_STRIP,
            EXIF_TAG_STRIP_BYTE_COUNTS,
            EXIF_TAG_X_RESOLUTION,
            EXIF_TAG_Y_RESOLUTION,
            EXIF_TAG_PLANAR_CONFIGURATION,
            EXIF_TAG_RESOLUTION_UNIT,
            EXIF_TAG_TRANSFER_FUNCTION,
            EXIF_TAG_SOFTWARE,
            EXIF_TAG_DATE_TIME,
            EXIF_TAG_ARTIST,
            EXIF_TAG_WHITE_POINT,
            EXIF_TAG_PRIMARY_CHROMATICITIES,
            EXIF_TAG_SUB_IFDS,
            EXIF_TAG_TRANSFER_RANGE,
            EXIF_TAG_JPEG_PROC,
            EXIF_TAG_JPEG_INTERCHANGE_FORMAT,
            EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH,
            EXIF_TAG_YCBCR_COEFFICIENTS,
            EXIF_TAG_YCBCR_SUB_SAMPLING,
            EXIF_TAG_YCBCR_POSITIONING,
            EXIF_TAG_REFERENCE_BLACK_WHITE,
            EXIF_TAG_XML_PACKET,
            EXIF_TAG_RELATED_IMAGE_FILE_FORMAT,
            EXIF_TAG_RELATED_IMAGE_WIDTH,
            EXIF_TAG_RELATED_IMAGE_LENGTH,
            EXIF_TAG_CFA_REPEAT_PATTERN_DIM,
            EXIF_TAG_CFA_PATTERN,
            EXIF_TAG_BATTERY_LEVEL,
            EXIF_TAG_COPYRIGHT,
            EXIF_TAG_EXPOSURE_TIME,
            EXIF_TAG_FNUMBER,
            EXIF_TAG_IPTC_NAA,
            EXIF_TAG_IMAGE_RESOURCES,
            EXIF_TAG_EXIF_IFD_POINTER,
            EXIF_TAG_INTER_COLOR_PROFILE,
            EXIF_TAG_EXPOSURE_PROGRAM,
            EXIF_TAG_SPECTRAL_SENSITIVITY,
            EXIF_TAG_GPS_INFO_IFD_POINTER,
            EXIF_TAG_ISO_SPEED_RATINGS,
            EXIF_TAG_OECF,
            EXIF_TAG_TIME_ZONE_OFFSET,
            EXIF_TAG_EXIF_VERSION,
            EXIF_TAG_DATE_TIME_ORIGINAL,
            EXIF_TAG_DATE_TIME_DIGITIZED,
            EXIF_TAG_COMPONENTS_CONFIGURATION,
            EXIF_TAG_COMPRESSED_BITS_PER_PIXEL,
            EXIF_TAG_SHUTTER_SPEED_VALUE,
            EXIF_TAG_APERTURE_VALUE,
            EXIF_TAG_BRIGHTNESS_VALUE,
            EXIF_TAG_EXPOSURE_BIAS_VALUE,
            EXIF_TAG_MAX_APERTURE_VALUE,
            EXIF_TAG_SUBJECT_DISTANCE,
            EXIF_TAG_METERING_MODE,
            EXIF_TAG_LIGHT_SOURCE,
            EXIF_TAG_FLASH,
            EXIF_TAG_FOCAL_LENGTH,
            EXIF_TAG_SUBJECT_AREA,
            EXIF_TAG_TIFF_EP_STANDARD_ID,
            EXIF_TAG_MAKER_NOTE,
            EXIF_TAG_USER_COMMENT,
            EXIF_TAG_SUB_SEC_TIME,
            EXIF_TAG_SUB_SEC_TIME_ORIGINAL,
            EXIF_TAG_SUB_SEC_TIME_DIGITIZED,
            EXIF_TAG_XP_TITLE,
            EXIF_TAG_XP_COMMENT,
            EXIF_TAG_XP_AUTHOR,
            EXIF_TAG_XP_KEYWORDS,
            EXIF_TAG_XP_SUBJECT,
            EXIF_TAG_FLASH_PIX_VERSION,
            EXIF_TAG_COLOR_SPACE,
            EXIF_TAG_PIXEL_X_DIMENSION,
            EXIF_TAG_PIXEL_Y_DIMENSION,
            EXIF_TAG_RELATED_SOUND_FILE,
            EXIF_TAG_INTEROPERABILITY_IFD_POINTER,
            EXIF_TAG_FLASH_ENERGY,
            EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE,
            EXIF_TAG_FOCAL_PLANE_X_RESOLUTION,
            EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION,
            EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT,
            EXIF_TAG_SUBJECT_LOCATION,
            EXIF_TAG_EXPOSURE_INDEX,
            EXIF_TAG_SENSING_METHOD,
            EXIF_TAG_FILE_SOURCE,
            EXIF_TAG_SCENE_TYPE,
            EXIF_TAG_NEW_CFA_PATTERN,
            EXIF_TAG_CUSTOM_RENDERED,
            EXIF_TAG_EXPOSURE_MODE,
            EXIF_TAG_WHITE_BALANCE,
            EXIF_TAG_DIGITAL_ZOOM_RATIO,
            EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM,
            EXIF_TAG_SCENE_CAPTURE_TYPE,
            EXIF_TAG_GAIN_CONTROL,
            EXIF_TAG_CONTRAST,
            EXIF_TAG_SATURATION,
            EXIF_TAG_SHARPNESS,
            EXIF_TAG_DEVICE_SETTING_DESCRIPTION,
            EXIF_TAG_SUBJECT_DISTANCE_RANGE,
            EXIF_TAG_IMAGE_UNIQUE_ID,
            EXIF_TAG_GAMMA,
            EXIF_TAG_PRINT_IMAGE_MATCHING,
            EXIF_TAG_PADDING,
        };
        for (int i = 0; i < EXIF_IFD_COUNT; i++)
        {
            for (auto tag : Tags)
            {
                printExifTag(ed, (ExifIfd)i, tag);
            }
        }
    }
#endif

}

cFormatJpeg::cFormatJpeg(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatJpeg::~cFormatJpeg()
{
}

bool cFormatJpeg::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, 4))
    {
        return false;
    }

    const auto h = reinterpret_cast<const uint8_t*>(buffer.data());

    const uint8_t a[4] = { 0xff, 0xd8, 0xff, 0xdb };
    if (::memcmp(h, a, sizeof(a)) == 0)
    {
        return true;
    }

    const uint8_t b[4] = { 0xff, 0xd8, 0xff, 0xe0 };
    if (::memcmp(h, b, sizeof(b)) == 0)
    {
        return true;
    }

    const uint8_t c[4] = { 0xff, 0xd8, 0xff, 0xe1 };
    if (::memcmp(h, c, sizeof(c)) == 0)
    {
        return true;
    }

    const uint8_t d[4] = { 0xff, 0xd8, 0xff, 0xfe };
    if (::memcmp(h, d, sizeof(d)) == 0)
    {
        return true;
    }

    return false;
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

    // unsigned exifSize = 0;
    // auto exif = locateExif(cinfo, exifSize);
    // if (exif != nullptr)
    // {
    // ::printf("-- exif located, size %u\n%s\n", exifSize, exif);
    // }

    // m_formatName = getFormat(cinfo.jpeg_color_space);
    m_formatName = m_cms.hasTransform() ? "jpeg/icc" : "jpeg";

    /* Step 4: set parameters for decompression */
    cinfo.out_color_space = JCS_RGB;    // convert to RGB

    /* Step 5: Start decompressor */
    jpeg_start_decompress(&cinfo);

    desc.width = cinfo.output_width;
    desc.height = cinfo.output_height;
    desc.bpp = cinfo.output_components * 8;
    desc.bppImage = cinfo.num_components * 8;
    desc.pitch = helpers::calculatePitch(desc.width, desc.bpp);
    desc.bitmap.resize(desc.pitch * desc.height);
    desc.format = GL_RGB;

    /* Step 6: while (scan lines remain to be read) */
    /*           jpeg_read_scanlines(...); */

    /* Here we use the library's state variable cinfo.output_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     */
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
            out += desc.pitch;

            updateProgress((float)cinfo.output_scanline / cinfo.output_height);
        }
    }
    else
    {
        std::vector<unsigned char> buffer(desc.pitch);
        auto input = buffer.data();
        while (cinfo.output_scanline < cinfo.output_height && m_stop == false)
        {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
            jpeg_read_scanlines(&cinfo, &input, 1);
            m_cms.doTransform(input, out, cinfo.output_width);
            out += desc.pitch;

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

    auto ed = exif_data_new_from_file(filename);
    if (ed != nullptr)
    {
#if defined(DEBUG)
        dumpAllExif(ed);
#endif

        auto& exifList = desc.exifList;

        addExifTag(ed, EXIF_IFD_0, EXIF_TAG_MAKE, exifList);
        addExifTag(ed, EXIF_IFD_0, EXIF_TAG_MODEL, exifList);
        addExifTag(ed, EXIF_IFD_0, EXIF_TAG_SOFTWARE, exifList);
        addExifTag(ed, EXIF_IFD_0, EXIF_TAG_ORIENTATION, exifList);
        addExifTag(ed, EXIF_IFD_0, EXIF_TAG_X_RESOLUTION, exifList);
        addExifTag(ed, EXIF_IFD_0, EXIF_TAG_Y_RESOLUTION, exifList);

        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_MAX_APERTURE_VALUE, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_FNUMBER, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_TIME, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_MODE, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_PROGRAM, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_X_DIMENSION, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_Y_DIMENSION, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_WHITE_BALANCE, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_COLOR_SPACE, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_DIGITAL_ZOOM_RATIO, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_FLASH, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_SCENE_CAPTURE_TYPE, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_CONTRAST, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_SATURATION, exifList);
        addExifTag(ed, EXIF_IFD_EXIF, EXIF_TAG_SHARPNESS, exifList);

        exif_data_unref(ed);
    }

    return true;
}
