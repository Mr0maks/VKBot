#pragma once

#include "dynamic_strings.h"
#include "vkapi.h"
#include "cmd_handler.h"

typedef struct
{
  void (*register_command)(const char *cmd_name, const char *description, cmd_function_callback callback);
  void (*vkapi_send_message)(vkapi_handle *object, int peer_id, const char *msg, vkapi_attach *attaches, int attaches_len);

  string_t (*string_init)( void );
  string_t (*string_dublicate)( string_t s );
  void (*string_format)( string_t s, const char *fmt, ...);
  void (*string_strncat)( string_t s, const char *string, size_t size );
  void (*string_copy)( string_t s, const char *string );
  void (*string_destroy)( string_t s );

  unsigned int (*crc32_calc) (const unsigned char *buf, size_t len);
} engine_api_t;
