#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdlib.h>

typedef struct{
  char *ptr;
  size_t len;
  size_t size;
} string_struct;

typedef string_struct * string_t;

string_t string_init( void );
string_t string_dublicate( string_t s );
void string_format( string_t s, const char *fmt, ...);
void string_strncat( string_t s, const char *string, size_t size );
void string_copy( string_t s, const char *string );
void string_destroy( string_t s );

#endif // STRING_H
