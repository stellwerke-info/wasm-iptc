#ifndef _LIBC_H
#define _LIBC_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#define WASM_EXPORT(n) __attribute__((export_name(n)))

void* memcpy(void* dest, const void* src, size_t count);
void* memset(void* dest, int value, size_t count);

void* malloc(size_t size);
#define emalloc(s) malloc(s)
void* realloc(void *ptr, size_t size);
void* calloc(size_t num, size_t size);
void free(void* mem);
#define efree(s) free(s)

#define EOF -1
typedef struct file_t {
  unsigned char* buf;
  size_t len;
  size_t pos;
} FILE;

int getc(FILE *fp);
#define fgetc(fp) getc(fp)
#define php_stream_getc(fp) getc(fp)

typedef struct file_t php_stream;
typedef int64_t zend_off_t;
typedef int64_t zend_long;
typedef int32_t ssize_t;

// Will be provided by Javascript.
void print_string(const char* str);

#endif
