#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

typedef unsigned char BYTE;

size_t base64_encode(const BYTE in[], BYTE out[], size_t len, int newline_flag);
size_t base64_decode(const BYTE in[], BYTE out[], size_t len);

#endif
