#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "dynamic_strings.h"
#include "vkapi.h"

typedef struct curl_keyvalue_s
{
	const char *key;
	const char *value;
	struct curl_keyvalue_s *next;
} curl_keyvalue_t;

typedef struct curl_postfield_s
{
	curl_keyvalue_t *pool;
} _curl_postfield_t;

typedef _curl_postfield_t * curl_postfield_t;

#ifndef _VKBOT_MODULE
size_t curl_dynamic_string_writefunc(void *ptr, size_t size, size_t nmemb, void *data);

curl_postfield_t curl_postfield_init(void);
void curl_postfield_push(curl_postfield_t pool, const char *key, const char *value);
string_t  curl_postfield_serialize(curl_postfield_t pool);
void curl_postfield_destroy(curl_postfield_t pool);

bool curl_get( void *curl_handle, const char *url, string_t useragent, string_t dataptr );
bool curl_post( void *curl_handle, const char *url, string_t post, string_t useragent, string_t dataptr );
bool curl_uploadfile( void *curl_handle, const char *url, const char *fieldname, const char *filename, string_t data, string_t useragent, string_t dataptr );

void curl_worker_share_init(void);
void curl_worker_share_deinit(void);

void *curl_init(void);
const char *curl_urlencode(const char *str);
void curl_wrap_free(void *ptr);
void curl_cleanup(void *ptr);
#endif
