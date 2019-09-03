#include "dynamic_strings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdarg.h>

#include <assert.h>

#include "gc_memmgr.h"

void _string_release(string_t s, void *ptr)
{
    string_destroy(s);
}

string_t string_init() {
  string_t s = (string_t)calloc( 1, sizeof(struct string) );

//  GC_REGISTER_FINALIZER(s, _string_release, NULL, NULL, NULL);
  
  s->ptr = (char *)malloc( 4096 );
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
  assert( s );

  string_t s_duble = string_init();

  if(s->len > 0)
    {
      s_duble->ptr = realloc( s_duble->ptr, s->size );
      memcpy( s_duble->ptr, s->ptr, s->len );
    }

  return s_duble;
}

void resize_string_if_need( string_t s, size_t size_need )
{
  assert( s );

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
  assert( s );

  char *ptr = NULL;
  int size = 0;
  va_list ap;

  va_start( ap, fmt );
  size = vsnprintf( ptr, size, fmt, ap );
  va_end( ap );

  if( size < 0 || size == 0 )
    return;

  if(s->size < size)
    {
  s->ptr = realloc( s->ptr, size + 1 );
  s->size = size + 1;
  s->len = 0;
    }

  if ( s->ptr == NULL ) {
      printf( "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }

  va_start( ap, fmt );
  size = vsnprintf( s->ptr, s->size, fmt, ap );
  va_end( ap );

  s->len = size;
  s->ptr[s->len] = '\0';
}

void _string_strncat_char( string_t s, const char *string, size_t size )
{
  assert( s );

  if(!string || size == 0)
    return;

  size_t new_len = s->len + size;

  s->ptr = realloc( s->ptr, new_len + 1 );

  if ( s->ptr == NULL ) {
      printf( "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }

  strncpy( s->ptr+s->len, string, size );
  s->ptr[new_len] = '\0';
  s->len = new_len;
  s->size = new_len + 1;
}

void _string_strncat_string( string_t s, string_t s2)
{
    assert( s );
    assert( s2 );

    _string_strncat_char( s, s2->ptr, s->len);
}

void string_memcpy( string_t s, const void *data, size_t size )
{
  assert( s );

  if(!data || size == 0)
    return;

  size_t new_len = s->len + size;

  s->ptr = realloc( s->ptr, new_len + 1 );

  if ( s->ptr == NULL ) {
      printf( "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }

  memcpy( s->ptr+s->len, data, size );
  //s->ptr[new_len] = '\0';
  s->len = new_len;
  s->size = new_len + 1;
}

void string_copy( string_t s, const char *string )
{
  assert( s );

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
