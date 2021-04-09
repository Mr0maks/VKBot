#pragma once

#ifndef _VKBOT_MODULE
#include <cJSON.h>
#endif

#include "dynamic_strings.h"
#include "curl_wrap.h"

#include <stdbool.h>

#define VK_URL_METHOD "https://api.vk.com/method/"
#define VK_API_VERSION "5.130"

#define VK_MESSAGE_LEN_MAX 4096
#define VK_MESSAGE_ATTACHMENS_MAX 10

typedef struct
{
  char vk_token[128];
  int group_id;

  char longpoll_server_url[128];
  int64_t longpoll_timestamp;
  char longpoll_key[128];

  void *curl_handle;
} vkapi_handle;

typedef struct
{
    string_t text;
    int peer_id;
    int from_id;
    bool private_message;
#ifndef _VKBOT_MODULE
    cJSON *attachmens;
#else
    void *attachmens;
#endif
} vkapi_reply_message;

typedef struct
{
  string_t text;
  int peer_id;
  int from_id;
  bool private_message;
  vkapi_reply_message *reply_message;
  #ifndef _VKBOT_MODULE
  cJSON *attachmens;
  #else
  void *attachmens;
  #endif
} vkapi_message_object;

typedef enum
{
  VKAPI_PHOTO,
  VKAPI_VIDEO,
  VKAPI_AUDIO,
  VKAPI_DOC,
  VKAPI_WALL,
  VKAPI_NULL
} docs_type_t;

typedef struct
{
  docs_type_t type;
  int owner_id;
  int media_id;
} vkapi_attachment;

#ifndef _VKBOT_MODULE
string_t vkapi_get_longpoll_data(vkapi_handle *object);
string_t vkapi_call_method(const char *method, curl_postfield_t args, bool result_need);
void vkapi_send_message(int peer_id, const char *msg, vkapi_attachment *attachments, int attachmens_len);
vkapi_attachment *vkapi_upload_attachment(vkapi_message_object *message, const char *filename, string_t data, docs_type_t type);
bool vkapi_get_long_poll_server(vkapi_handle *object);
vkapi_handle *vkapi_init(const char *token);
void vkapi_destroy(vkapi_handle *ptr);
#endif
