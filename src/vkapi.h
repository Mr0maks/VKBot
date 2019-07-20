#pragma once

#include <curl/curl.h>
#include <cJSON.h>
#include "vk_strings.h"

typedef struct vkapi_object_s
{
  char vk_token[128];
  int group_id;

  char longpoll_server_url[128];
  int longpoll_timestamp;
  char longpoll_key[128];

  CURL *curl_handle;
} vkapi_object;

typedef struct
{
  string_t text;
  int peer_id;
  int from_id;
  cJSON *attachmens;
} vkapi_message_new_object;

typedef enum
{
  false,
  true
} vkapi_bool;

string_t vkapi_get_longpoll_data(vkapi_object *object);
string_t vk_api_call_method(vkapi_object *object, const char *method, string_t specific_args, vkapi_bool result_need);
void vkapi_send_message(vkapi_object *object, int peer_id, const char *msg);
vkapi_bool vkapi_get_long_poll_server(vkapi_object *object);
vkapi_object *vk_api_init(const char *token, int group_id);
