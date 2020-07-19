#pragma once

#include <stdint.h>
#include <unistd.h>

uint32_t memcrc32 (const unsigned char *buf, size_t len);
int memcrc32cmp(const unsigned char *buf1 , const unsigned char *buf2, size_t len);
#define strncrc32(ptr, len) memcrc32((void*)ptr, len)
int strncrc32cmp( const char *str1 , const char *str2, size_t len );
