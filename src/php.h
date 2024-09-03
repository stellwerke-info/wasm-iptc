#ifndef _PHP_H
#define _PHP_H

#include "libc.h"

#define emalloc(s) malloc(s)
#define efree(s) free(s)

#define php_stream_getc(fp) getc(fp)

typedef struct file_t php_stream;
typedef int64_t zend_off_t;
typedef int64_t zend_long;
typedef int32_t ssize_t;

void php_stream_seek_cur(php_stream * stream, zend_long length);
ssize_t php_stream_read(php_stream * stream, void * buf, size_t length);

#endif
