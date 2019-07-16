#pragma once

#include <curl/curl.h>
#include "strings.h"

typedef struct vkapi_object_s
{
  char vk_token[128];
  char group_id[128];

  char longpool_server_url[128];
  int longpool_timestamp;
  char longpool_key[128];

  CURL *curl_handle;
} vkapi_object;

typedef enum
{
  false,
  true
} bool;

struct string *vkapi_get_longpool_data(vkapi_object *object);

struct string *vk_api_call_method(vkapi_object *object, const char *method, const char *args);

vkapi_object *vk_api_init(const char *token, const char *group_id);
