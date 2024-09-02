#include "libc.h"

#define PAGE_SIZE (64 * 1024)

void* sbrk(intptr_t increment) {
  void* brk = (void*)(__builtin_wasm_memory_size(0) * PAGE_SIZE);
  if (increment == 0)
    return brk;
  if (__builtin_wasm_memory_grow(0, increment / PAGE_SIZE + 1) == SIZE_MAX)
    return (void*) -1;
  return brk;
}

void* memcpy(void* dest, const void* src, size_t count) {
  return __builtin_memcpy(dest, src, count);
}

void* memset(void * dest, int value, size_t count) {
  return __builtin_memset(dest, value, count);
}

void* malloc(size_t size) {
  if (size == 0) { return NULL; }
  void *request = sbrk(size);
  if (request == (void*) -1) return NULL;
  return request;
}

void* realloc(void *ptr, size_t size) {
  return malloc(size);
}

void* calloc(size_t num, size_t size) {
  size_t total = num * size;
  void* mem = malloc(total);
  memset(mem, 0, total);
  return mem;
}

void free(void* mem) {}

int getc(FILE *fp) {
  if (fp->pos > fp->len) return EOF;
  return fp->buf[fp->pos++];
}
