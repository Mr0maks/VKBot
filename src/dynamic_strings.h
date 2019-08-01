#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdlib.h>

struct string {
  char *ptr;
  size_t len;
  size_t size;
};

typedef struct string* string_t;

#ifndef _VKBOT_MODULE
string_t string_init( void );
string_t string_dublicate( string_t s );
void string_format( string_t s, const char *fmt, ...);
void string_strncat( string_t s, const char *string, size_t size );
void string_copy( string_t s, const char *string );
void string_destroy( string_t s );
#else
#define string_init (*engine_api.string_init)
#define string_dublicate (*engine_api.string_dublicate)
#define string_format (*engine_api.string_format)
#define string_strncat (*engine_api.string_strncat)
#define string_copy (*engine_api.string_copy)
#define string_destroy (*engine_api.string_destroy)
#endif

#endif // STRING_H
