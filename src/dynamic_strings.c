/*
dynamic_string.c - Dynamic strings
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "common.h"

#define DYN_STRING_SIZE 1024

string_t string_init() {
  string_t s = (string_t)calloc( 1, sizeof(struct string) );

  s->ptr = (char *)malloc( DYN_STRING_SIZE );
  if ( s->ptr == NULL ) {
      Con_Printf( "malloc() failed\n" );
      exit( EXIT_FAILURE );
    }
  
  s->ptr[0] = '\0';
  s->len = 0;
  s->size = DYN_STRING_SIZE;
  return s;
}

string_t string_dublicate(string_t s)
{
  assert( s );

  string_t s_duble = string_init();

  if(s->len > 0)
    {
      s_duble->ptr = realloc( s_duble->ptr, s->size );
	  memcpy( s_duble->ptr, s->ptr, s->len + 1 );
    }

  return s_duble;
}

static void resize_string_if_need( string_t s, size_t size_need )
{
  assert( s );

  if( size_need == 0 )
    return;

  if( size_need < s->len )
    return;

  s->ptr = realloc( s->ptr, size_need + 1 );

  if ( s->ptr == NULL ) {
      Con_Printf( "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }
  
  s->ptr[size_need] = '\0';

  s->len = size_need;
}

void string_format( string_t s, const char *fmt, ...)
{
  assert( s );

  char *ptr = NULL;
  size_t size = 0;
  va_list ap;

  va_start( ap, fmt );
  size = (size_t)vsnprintf( ptr, size, fmt, ap );
  va_end( ap );

  assert(size != (size_t)-1);

  if( size == 0 )
    return;

  if(s->size < size)
    {
  s->ptr = realloc( s->ptr, size + 1 );
  s->size = size + 1;
  s->len = 0;
    }

  if ( s->ptr == NULL ) {
      Con_Printf( "realloc() failed\n" );
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
  assert( string );
  assert( size != 0 );

  if(!string || size == 0)
    return;

  size_t new_len = s->len + size;

  s->ptr = realloc( s->ptr, new_len + 1 );

  if ( s->ptr == NULL ) {
      Con_Printf( "realloc() failed\n" );
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
  assert( data );
  assert( size != 0 );

  if(!data || size == 0)
    return;

  size_t new_len = s->len + size;

  s->ptr = realloc( s->ptr, new_len + 1 );

  if ( s->ptr == NULL ) {
      Con_Printf( "realloc() failed\n" );
      exit( EXIT_FAILURE );
    }

  memcpy( s->ptr+s->len, data, size );
  s->ptr[new_len] = '\0';
  s->len = new_len;
  s->size = new_len + 1;
}

void string_copy( string_t s, const char *string )
{
  assert( s );

  size_t slen = strlen( string ) + 1;

  resize_string_if_need( s, slen );

  strncpy( s->ptr, string, slen + 1 );
}

void string_destroy( string_t s )
{
    assert(s);

  if( s->ptr != NULL )
    free( s->ptr );

  free( s );
}
