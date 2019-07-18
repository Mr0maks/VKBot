#include "vk_strings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdarg.h>

#include <assert.h>

struct string *init_string() {
  struct string *s = (struct string*)malloc(sizeof(struct string));
  
  s->ptr = (char*)malloc(8192 * 2);
  if (s->ptr == NULL) {
      printf("malloc() failed\n");
      exit(EXIT_FAILURE);
    }
  
  s->ptr[0] = '\0';
  
  s->len = 0;
  return s;
}

struct string *dublicate_string(struct string *s)
{
  struct string *s_duble = init_string();

  s_duble->ptr = realloc(s_duble->ptr, s->len);

  memcpy(s_duble->ptr, s->ptr, s->len);

  return s_duble;
}

void resize_string_if_need( struct string *s, size_t size_need )
{
  if( size_need == 0 )
    return;

  if( size_need < s->len )
    return;

  s->ptr = realloc( s->ptr, size_need + 1 );

  if (s->ptr == NULL) {
      printf("realloc() failed\n");
      exit(EXIT_FAILURE);
    }
  
  s->ptr[size_need] = '\0';

  s->len = size_need;
}

void string_format( struct string *s, const char *fmt, ...)
{
  char *ptr = NULL;
  int size = 0;
  va_list ap;

  va_start( ap, fmt );
  size = vsnprintf( ptr, size, fmt, ap );
  va_end( ap );

  if( size < 0 || size == 0 )
    return;

  s->ptr = realloc(s->ptr, size+1);

  if (s->ptr == NULL) {
      fprintf(stderr, "realloc() failed\n");
      exit(EXIT_FAILURE);
    }

  va_start( ap, fmt );
  size = vsnprintf( s->ptr, size, fmt, ap );
  va_end( ap );

}

void strncat_to_string( struct string *s, const char *string, size_t size )
{
  assert(s != NULL);

  size_t new_len = s->len + size;

  s->ptr = realloc(s->ptr, new_len+1);

  if (s->ptr == NULL) {
  fprintf(stderr, "realloc() failed\n");
  exit(EXIT_FAILURE);
  }

  strncpy(s->ptr+s->len, string, size);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  s->len = new_len;
}

void push_to_string( struct string *s, const char *string )
{
  size_t slen = strlen( string );

  resize_string_if_need( s, slen );

  strncpy( s->ptr, string, slen );
  s->ptr[slen] = '\0';

  s->len = slen;
}

void destroy_string( struct string *s )
{
  if( !s )
    return;

  if( s->ptr )
    free( s->ptr );

  free( s );
}
