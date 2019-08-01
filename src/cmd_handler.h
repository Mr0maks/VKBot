#pragma once

#include "vkapi.h"
#include "users.h"

typedef  void	(*cmd_function_callback)(vkapi_handle *object, vkapi_message_object *message, int argc, char *argv[], const char *args);

typedef struct
{
  const char	*string;
  const char    *description;
//  privilage_t	privilage;
  cmd_function_callback  function;
} cmds_t;

typedef struct
{
  unsigned int hash;
  cmd_function_callback  function;
} cmds_hashs_t;

typedef struct
{
  const char *name;
} cmds_names_t;

typedef struct
{
  unsigned int hash;
} cmds_name_hashs_t;

typedef struct cmds_modules_pools_s
{
  int module_id;
  const char	*string;
  const char    *description;
  cmd_function_callback  function;
  unsigned int hash;
  struct cmds_modules_pools_s *next;
} cmds_modules_pools_t;

#ifndef _VKBOT_MODULE
vkapi_bool cmd_handle(vkapi_handle *object, vkapi_message_object *message);
void cmd_handler_deinit(void);
void cmd_handler_init(void);
#endif
