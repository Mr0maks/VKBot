#pragma once

#include <stdlib.h>

unsigned int memcrc32 (const unsigned char *buf, size_t len);
int memcrc32cmp(const unsigned char *buf1 , const unsigned char *buf2, size_t len);
unsigned int strcrc32 (const char *buf);
unsigned int strcrc32case(const char *buf);
unsigned int strncrc32 (const char *buf, size_t len);
unsigned int strncrc32case(const char *buf, size_t len);
int strcrc32cmp( const char *str1 , const char *str2 );
int strncrc32cmp( const char *str1 , const char *str2, size_t len );
int strcrc32casecmp(const char *str1 , const char *str2);
int strncrc32casecmp(const char *str1 , const char *str2, size_t len);
