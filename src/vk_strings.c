#include "vk_strings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdarg.h>

#include <assert.h>

string_t string_init() {
  string_t s = (string_t)calloc( 1, sizeof(string_struct) );
  
  s->ptr = (char *)calloc( 2, 2048 );
  if ( s->ptr == NULL ) {
      printf( "сalloc() failed\n" );
      exit( EXIT_FAILURE );
    }
  
  s->ptr[0] = '\0';
  s->len = 0;
  s->size = 4096;
  return s;
}

string_t string_dublicate(string_t s)
{
  assert( s != NULL );

  string_t s_duble = string_init();

  if(s->len > 0)
    {
      s_duble->ptr = realloc( s_duble->ptr, s->len );
      memcpy( s_duble->ptr, s->ptr, s->len );
    }

  return s_duble;
}

void resize_string_if_need( string_t s, size_t size_need )
{
  assert( s != NULL );

  if( size_need == 0 )
    return;

  if( size_need < s->len )
    return;

  s->ptr = realloc( s->ptr, size_need + 1 );

  if ( s->ptr == NULL ) {
      printf( "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }
  
  s->ptr[size_need] = '\0';

  s->len = size_need;
}

void string_format( string_t s, const char *fmt, ...)
{
  assert( s != NULL );

  char *ptr = NULL;
  int size = 0;
  va_list ap;

  va_start( ap, fmt );
  size = vsnprintf( ptr, size, fmt, ap );
  va_end( ap );

  if( size < 0 || size == 0 )
    return;

  //s->ptr = realloc(s->ptr, size+2);

  if ( s->ptr == NULL ) {
      fprintf( stderr, "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }

  va_start( ap, fmt );
  size = vsnprintf( s->ptr, s->size, fmt, ap );
  va_end( ap );

  s->len = size;
  s->ptr[s->len] = '\0';
}

void string_strncat( string_t s, const char *string, size_t size )
{
  assert( s != NULL );

  if(!string || size == 0)
    return;

  size_t new_len = s->len + size;

  if( (s->size - s->len) < size )
     s->ptr = realloc(s->ptr, new_len+1);

  if ( s->ptr == NULL ) {
  fprintf( stderr, "realloc() failed\n" );
  exit( EXIT_FAILURE );
  }

  strncpy( s->ptr+s->len, string, size );
  s->ptr[new_len] = '\0';
  s->len = new_len;
}

void string_copy( string_t s, const char *string )
{
  assert( s != NULL );

  size_t slen = strlen( string );

  resize_string_if_need( s, slen );

  strncpy( s->ptr, string, slen );
  s->ptr[slen] = '\0';

  s->len = slen;
}

void string_destroy( string_t s )
{
  if( !s )
    return;

  if( s->ptr )
    free( s->ptr );

  free( s );
}
