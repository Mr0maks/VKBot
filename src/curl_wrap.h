#pragma once

#include <stddef.h>
#include "dynamic_strings.h"
#include "vkapi.h"

#ifndef _VKBOT_MODULE
size_t curl_dynamic_string_writefunc(void *ptr, size_t size, size_t nmemb, void *data);

bool curl_get( void *curl_handle, string_t url, string_t useragent, string_t dataptr );
bool curl_post(void *curl_handle, const char *url, string_t post, string_t useragent, string_t dataptr );
bool curl_uploadfile( void *curl_handle, const char *url, const char *fieldname, const char *filename, string_t data, string_t useragent, string_t dataptr );
void *curl_init(void);
char *curl_urlencode(const char *data);
void curl_ptr_free(void *ptr);
void curl_cleanup(void *ptr);
#endif
