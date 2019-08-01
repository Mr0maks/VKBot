#include "memcache.h"
#include <stdlib.h>
#include <assert.h>

#include <string.h>

#include <stdint.h>

#include "crc32_hash.h"

typedef struct
{
  memcache_t *list;
  size_t max_size;
  size_t size_now;
} memcache_pool_t;

memcache_pool_t *poll = NULL;

memcache_t *memcache_find(const char *key)
{
  if(!key)
    return NULL;

  memcache_t *ptr = poll->list;

  uint32_t hash = crc32_calc((const unsigned char *)key, strlen(key));

  while (ptr) {

      if(ptr->hash == hash)
	return ptr;

      ptr = ptr->next;
    }

  return NULL;
}

void memcache_init(size_t size)
{
  poll = malloc(sizeof(memcache_pool_t));

  poll->max_size = ((size * 1024) * 1024);

  poll->size_now = 0;
}
