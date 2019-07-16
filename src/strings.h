#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stdlib.h>

struct string {
  char *ptr;
  size_t len;
};

struct string *init_string();

void strncat_to_string( struct string *s, const char *string, size_t size );

void push_to_string( struct string *s, const char *string );

void destroy_string( struct string *s );

#endif // STRING_H
