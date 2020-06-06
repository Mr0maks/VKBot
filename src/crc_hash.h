#pragma once

#include <stdint.h>
#include <unistd.h>

uint32_t memcrc32 (const unsigned char *buf, size_t len);
int memcrc32cmp(const unsigned char *buf1 , const unsigned char *buf2, size_t len);
uint32_t strncrc32 (const char *buf, size_t len);
uint32_t strncrc32case(const char *buf, size_t len);
int strcrc32cmp( const char *str1 , const char *str2 );
int strncrc32cmp( const char *str1 , const char *str2, size_t len );
int strcrc32casecmp(const char *str1 , const char *str2);
int strncrc32casecmp(const char *str1 , const char *str2, size_t len);
