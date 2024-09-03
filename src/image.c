/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */
 
#include "libc.h"
#include "php.h"

// Will be provided by Javascript.
void read_app_cb(int, char*, size_t);

/* file type markers */
const char php_sig_jpg[3] = {(char) 0xff, (char) 0xd8, (char) 0xff};

/* routines to handle JPEG data */

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0			/* Start Of Frame N */
#define M_SOF1  0xC1			/* N indicates which compression process */
#define M_SOF2  0xC2			/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5			/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8
#define M_EOI   0xD9			/* End Of Image (end of datastream) */
#define M_SOS   0xDA			/* Start Of Scan (begins compressed data) */
#define M_APP0  0xe0
#define M_APP1  0xe1
#define M_APP2  0xe2
#define M_APP3  0xe3
#define M_APP4  0xe4
#define M_APP5  0xe5
#define M_APP6  0xe6
#define M_APP7  0xe7
#define M_APP8  0xe8
#define M_APP9  0xe9
#define M_APP10 0xea
#define M_APP11 0xeb
#define M_APP12 0xec
#define M_APP13 0xed
#define M_APP14 0xee
#define M_APP15 0xef
#define M_COM   0xFE            /* COMment                                  */

#define M_PSEUDO 0xFFD8			/* pseudo marker for start of image(byte 0) */

/* {{{ php_read2 */
static unsigned short php_read2(php_stream * stream)
{
	unsigned char a[2];

	/* return 0 if we couldn't read enough data */
	if((php_stream_read(stream, (char *) a, sizeof(a))) < sizeof(a)) return 0;

	return (((unsigned short)a[0]) << 8) + ((unsigned short)a[1]);
}
/* }}} */

/* {{{ php_next_marker
 * get next marker byte from file */
static unsigned int php_next_marker(php_stream * stream, int last_marker, int ff_read)
{
	int a=0, marker;

	/* get marker byte, swallowing possible padding                           */
	if (!ff_read) {
		size_t extraneous = 0;

		while ((marker = php_stream_getc(stream)) != 0xff) {
			if (marker == EOF) {
				return M_EOI;/* we hit EOF */
	}
			extraneous++;
	}
		if (extraneous) {
			//php_error_docref("Corrupt JPEG data: %zu extraneous bytes before marker", extraneous);
		}
	}
	a = 1;
	do {
		if ((marker = php_stream_getc(stream)) == EOF)
		{
			return M_EOI;/* we hit EOF */
		}
		a++;
	} while (marker == 0xff);
	if (a < 2)
	{
		return M_EOI; /* at least one 0xff is needed before marker code */
	}
	return (unsigned int)marker;
}
/* }}} */

/* {{{ php_skip_variable
 * skip over a variable-length block; assumes proper length marker */
static int php_skip_variable(php_stream * stream)
{
	zend_off_t length = ((unsigned int)php_read2(stream));

	if (length < 2)	{
		return 0;
	}
	length = length - 2;
	php_stream_seek_cur(stream, (zend_long)length);
	return 1;
}
/* }}} */

static size_t php_read_stream_all_chunks(php_stream *stream, char *buffer, size_t length)
{
	return php_stream_read(stream, buffer, length);
}

/* {{{ php_read_APP */
static int php_read_APP(php_stream * stream, unsigned int marker)
{
	size_t length;
	char *buffer;

	length = php_read2(stream);
	if (length < 2)	{
		return 0;
	}
	length -= 2;				/* length includes itself */

	buffer = emalloc(length);

	if (php_read_stream_all_chunks(stream, buffer, length) != length) {
		efree(buffer);
		return 0;
	}

	int app_marker_number = marker - M_APP0;
	read_app_cb(app_marker_number, buffer, length);

        // note: do not free here, willl be used later.
	// efree(buffer);
	return 1;
}
/* }}} */

/* {{{ php_handle_jpeg
   main loop to parse JPEG structure */
static void php_handle_jpeg (php_stream * stream)
{
	unsigned int marker = M_PSEUDO;
	unsigned short ff_read=1;

	for (;;) {
		marker = php_next_marker(stream, marker, ff_read);
		ff_read = 0;
		switch (marker) {
			case M_SOF0:
			case M_SOF1:
			case M_SOF2:
			case M_SOF3:
			case M_SOF5:
			case M_SOF6:
			case M_SOF7:
			case M_SOF9:
			case M_SOF10:
			case M_SOF11:
			case M_SOF13:
			case M_SOF14:
			case M_SOF15:
				if (!php_skip_variable(stream)) {
					return;
				}
				break;

			case M_APP0:
			case M_APP1:
			case M_APP2:
			case M_APP3:
			case M_APP4:
			case M_APP5:
			case M_APP6:
			case M_APP7:
			case M_APP8:
			case M_APP9:
			case M_APP10:
			case M_APP11:
			case M_APP12:
			case M_APP13:
			case M_APP14:
			case M_APP15:
				if (!php_read_APP(stream, marker)) { /* read all the app marks... */
					return;
				}
				break;

			case M_SOS:
			case M_EOI:
				return;	/* we're about to hit image data, or are at EOF. stop processing. */

			default:
				if (!php_skip_variable(stream)) { /* anything else isn't interesting */
					return;
				}
				break;
		}
	}
}
/* }}} */
WASM_EXPORT("jpeg_iter_app") bool jpeg_iter_app(unsigned char* jpeg_file, size_t jpeg_file_len) /* {{{ */
{
  php_stream *stream = (FILE*)calloc(1, sizeof(FILE));
  stream->buf = jpeg_file;
  stream->len = jpeg_file_len;

	char filetype[3];
	if((php_stream_read(stream, filetype, 3)) != 3) {
		return false;
	}

        if (filetype[0] != php_sig_jpg[0] || filetype[1] != php_sig_jpg[1] || filetype[2] != php_sig_jpg[2]) {
          return false;
        }

	php_handle_jpeg(stream);
	return true;
}
/* }}} */
