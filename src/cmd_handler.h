#pragma once

#include "vkapi.h"

typedef  void	(*cmd_function)(vkapi_object *object, vkapi_message_new_object *message, int argc, char *argv[], const char *args);

typedef struct
{
  const char	*string;
  const char    *description;
  cmd_function  function;
} cmds_t;

typedef struct
{
  unsigned int hash;
  cmd_function  function;
} cmds_hashs_t;

typedef struct
{
  const char *name;
} cmds_names_t;

typedef struct
{
  unsigned int hash;
} cmds_name_hashs_t;

vkapi_bool cmd_handle(vkapi_object *object, vkapi_message_new_object *message);
void cmd_handler_init(void);
