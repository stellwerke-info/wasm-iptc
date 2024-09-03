#include "php.h"

void php_stream_seek_cur(php_stream * stream, zend_long length)
{
  int remaining = stream->len - stream->pos;
  if (length > remaining) length = remaining;
  stream->pos += length;
}

ssize_t php_stream_read(php_stream * stream, void * buf, size_t length)
{
  int remaining = stream->len - stream->pos;
  if (length > remaining) length = remaining;
  memcpy(buf, stream->buf + stream->pos, length);
  stream->pos += length;
  return length;
}
