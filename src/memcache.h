#pragma once

#include <stdlib.h>

typedef struct memcache_s
{
  unsigned int hash;
  int count_access;
  char *data;
  size_t data_len;
  struct memcache_s *next;
} memcache_t;

void memcache_push(const char *key, const char *value);
size_t memcache_get_max_size(void);
size_t memcache_get_size(void);
const char* memcache_get(const char *key);
void memcache_init(size_t size);
