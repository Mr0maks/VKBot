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
