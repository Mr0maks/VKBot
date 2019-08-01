#pragma once

typedef struct memcache_s
{
  unsigned int hash;
  char *ptr;
  struct memcache_s *next;
} memcache_t;
