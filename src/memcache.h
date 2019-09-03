#pragma once

#include <stddef.h>

#ifndef _VKBOT_MODULE
void memcache_push(const char *key, const char *value);
size_t memcache_get_max_size(void);
size_t memcache_get_size(void);
const char* memcache_get(const char *key);
void memcache_init(size_t size);
#endif
