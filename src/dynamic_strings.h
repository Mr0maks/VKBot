#pragma once

#include <stdint.h>
#include <stdlib.h>

struct string {
  char *ptr;
  size_t len;
  size_t size;
};

typedef struct string* string_t;

struct string_lib
{
  string_t (*init)( void );
  string_t (*dublicate)( string_t s );
  void (*format)( string_t s, const char *fmt, ...);
  void (*strncat)( string_t s, const char *string, size_t size );
  void (*copy)( string_t s, const char *string );
  void (*destroy)( string_t s );
};

#ifndef _VKBOT_MODULE
string_t string_init( void );
string_t string_dublicate( string_t s );
void string_format( string_t s, const char *fmt, ...);
void string_strncat( string_t s, const char *string, size_t size );
void string_copy( string_t s, const char *string );
void string_destroy( string_t s );
#endif
