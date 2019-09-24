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
void _string_strncat_char( string_t s, const char *string, size_t size );
void _string_strncat_string( string_t s, string_t s2);
void string_copy( string_t s, const char *string );
#define string_strncat(x, x2, x3) _string_strncat_char(x, x2, x3)
/*#define string_strncat( x, x2, x3 ) _Generic((x2), \
    char* : _string_strncat_char(x, x2, x3), \
    const char* : _string_strncat_char(x, x2, x3), \
    struct string * : _string_strncat_string(x, x2) \
    )
*/

void string_destroy( string_t s );
#endif
