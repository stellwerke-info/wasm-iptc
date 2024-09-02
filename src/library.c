#include "libc.h"

WASM_EXPORT("alloc_buf") unsigned char* alloc_buf(size_t size) {
  return (unsigned char*) calloc(size, 1);
}

/*WASM_EXPORT("free_buf") void free_buf(char* str) {
  free(str);
}*/

WASM_EXPORT("file_len") size_t file_len(FILE* fp) { return fp->len; }
WASM_EXPORT("file_ptr") unsigned char* file_ptr(FILE* fp) { return fp->buf; }
