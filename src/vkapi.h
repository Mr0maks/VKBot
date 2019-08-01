#pragma once

#ifndef _VKBOT_MODULE
#include <curl/curl.h>
#include <cJSON.h>
#endif

#include "dynamic_strings.h"

#ifndef _VKBOT_MODULE
typedef struct
{
  char vk_token[128];
  int group_id;

  char longpoll_server_url[128];
  int64_t longpoll_timestamp;
  char longpoll_key[128];

  CURL *curl_handle;
} vkapi_handle;
#else
typedef void vkapi_object;
#endif

typedef struct
{
  string_t text;
  int peer_id;
  int from_id;
  #ifndef _VKBOT_MODULE
  cJSON *attachmens;
  #else
  void *attachmens;
  #endif
} vkapi_message_object;

typedef enum
{
  false,
  true
} vkapi_bool;

#ifndef _VKBOT_MODULE
string_t vkapi_get_longpoll_data(vkapi_handle *object);
string_t vkapi_call_method(vkapi_handle *object, const char *method, string_t specific_args, vkapi_bool result_need);
void vkapi_send_message(vkapi_handle *object, int peer_id, const char *msg);
vkapi_bool vkapi_get_long_poll_server(vkapi_handle *object);
vkapi_handle *vkapi_init(const char *token, int group_id);
void vkapi_destroy(vkapi_handle *ptr);
#endif
