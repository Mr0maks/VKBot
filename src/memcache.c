#include "common.h"
#include "memcache.h"
#include "crc_hash.h"

typedef struct memcache_s
{
  unsigned int hash;
  int count_access;
  char *data;
  size_t data_len;
  struct memcache_s *next;
} memcache_t;

typedef struct
{
  memcache_t *list;
  size_t max_size;
  size_t size_now;
} memcache_pool_t;

static memcache_pool_t *pool = NULL;

memcache_t *memcache_find(const char *key)
{
  if(!key)
    return NULL;

  memcache_t *ptr = pool->list;

  uint32_t hash = crc32_calc((const unsigned char *)key, strlen(key));

  while (ptr) {

      if(ptr->hash == hash)
	return ptr;

      ptr = ptr->next;
    }

  return NULL;
}

memcache_t *memcache_alloc(unsigned int hash, const char *data, size_t data_len)
{
  if(pool->max_size - pool->size_now < data_len)
    return NULL;

  memcache_t *ptr = malloc(sizeof(memcache_t));

  if(!ptr)
    return NULL;

  ptr->hash = hash;
  ptr->count_access = 0;
  ptr->data = malloc(data_len + 1);
  ptr->data_len = data_len;

  ptr->data[data_len] = '\0';

  strncpy(ptr->data, data, data_len);

  pool->size_now += data_len;

  return ptr;
}

void memcache_delete( memcache_t *plugin )
{
  memcache_t *ptr = pool->list, *ptr_t1 = NULL;
  if(ptr->hash == plugin->hash)
    {
      pool->list = ptr->next;
      return;
    }

  ptr_t1 = ptr->next;

  while(ptr_t1)
    {

      if( ptr_t1->hash == plugin->hash )
	{
	  ptr->next = ptr_t1->next;
	  return;
	}

      ptr = ptr_t1;
      ptr_t1 = ptr_t1->next;
    }
}


memcache_t *memcache_realloc(memcache_t *ptr, const char *data, size_t data_len)
{
  if(((pool->max_size - pool->size_now) - ptr->data_len) < data_len)
    return NULL;

  ptr->data = realloc(ptr->data, data_len + 1);
  if(!ptr->data)
    {
      Con_Printf("realloc failed\n");
      exit(EXIT_FAILURE);
    }
  ptr->data_len = data_len;
  ptr->data[data_len] = '\0';

  strncpy(ptr->data, data, data_len);

  return ptr;
}

void memcache_push(const char *key, const char *value)
{
  if(!key || !value)
  {
      return;
  }

    memcache_t *ptr = memcache_find(key);

  if(ptr)
    {
      memcache_realloc(ptr, value, strlen(value));
	return;
    } else {
      unsigned int hash = crc32_calc((const unsigned char *)key, strlen(key));
      ptr = memcache_alloc(hash, value, strlen(value));
      ptr->next = pool->list;
      pool->list = ptr;
    }
}

size_t memcache_get_max_size()
{
  return pool->max_size / 1024;
}
size_t memcache_get_size()
{
  return pool->size_now;
}

const char* memcache_get(const char *key)
{
  if(!key)
    return NULL;

  memcache_t *ptr = memcache_find(key);

  if(!ptr)
    return NULL;

  ptr->count_access++;

  return ptr->data;
}

void memcache_init(size_t size)
{
  pool = malloc(sizeof(memcache_pool_t));
  pool->list = NULL;
  pool->max_size = ((size * 1024) * 1024);
  pool->size_now = 0;
}
