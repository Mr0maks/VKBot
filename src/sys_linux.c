#include <stdio.h>
#include <string.h>

#include "vk_strings.h"

int get_param_of_path(const char *filenpath, const char *what)
{
  string_t s = string_init();

  char buffer[32];

  FILE *f = fopen(filenpath, "r");

  if(!f)
    return 0;

  fread( s->ptr, 1, s->size, f );

  fclose( f );

  char *what_value = strstr( s->ptr, what );

  while( *what_value < '0' || *what_value > '9'  )
    *what_value++;


  if( what_value )
    {
    strncpy(buffer, what_value, 32);
    string_destroy( s );
    return atoi( buffer );
    }

  string_destroy( s );
  return 0;
}
