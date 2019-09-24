#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#include "dynamic_strings.h"

double get_time_s( void )
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)(ts.tv_sec) + (double) ts.tv_nsec/1000000000.0;
}


long get_param_of_path(const char *filenpath, const char *what)
{
  string_t s = string_init();
  char *endptr = NULL;

  FILE *f = fopen(filenpath, "r");

  if(!f)
    return 0;

  (void)fread( s->ptr, 1, s->size, f );

  fclose( f );

  const char *what_value = strstr( s->ptr, what );

  while( *what_value < '0' || *what_value > '9'  )
    what_value++;

  if( what_value )
    {
    long value = strtol(what_value, &endptr, 0);
    string_destroy( s );
    return value;
    }

  string_destroy( s );
  return 0;
}
