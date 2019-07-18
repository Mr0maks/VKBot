#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

typedef unsigned char BYTE;

size_t encode(const BYTE in[], BYTE out[], size_t len, int newline_flag);
size_t decode(const BYTE in[], BYTE out[], size_t len);

#endif
