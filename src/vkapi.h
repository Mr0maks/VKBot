#pragma once

#ifndef _VKBOT_MODULE
#include <cJSON.h>
#endif

#include "dynamic_strings.h"

typedef enum __attribute__((packed))
{
  false,
  true
} vkapi_boolean;

#ifndef _VKBOT_MODULE
typedef struct
{
  char vk_token[128];
  int group_id;

  char longpoll_server_url[128];
  int64_t longpoll_timestamp;
  char longpoll_key[128];

  void *curl_handle;
} vkapi_handle;
#else
typedef void vkapi_handle;
#endif

typedef struct
{
  string_t text;
  int peer_id;
  int from_id;
  vkapi_boolean private_message;
  #ifndef _VKBOT_MODULE
  cJSON *attachmens;
  #else
  void *attachmens;
  #endif
} vkapi_message_object;

#ifndef _VKBOT_MODULE
string_t vkapi_get_longpoll_data(vkapi_handle *object);
string_t vkapi_call_method(vkapi_handle *object, const char *method, string_t specific_args, vkapi_boolean result_need);
void vkapi_send_message(vkapi_handle *object, int peer_id, const char *msg);
vkapi_boolean vkapi_get_long_poll_server(vkapi_handle *object);
vkapi_handle *vkapi_init(const char *token, int group_id);
void vkapi_destroy(vkapi_handle *ptr);
#endif
