#pragma once

#include "vkapi.h"
#include "module.h"

typedef  void	(*cmd_callback)(vkapi_message_object *message, int argc, char *argv[], const char *args);

typedef struct
{
  const char	*string;
  const char    *description;
  cmd_callback  function;
} cmds_t;

#ifndef _VKBOT_MODULE
bool cmd_handle(vkapi_message_object *message);
//void cmd_handler_module_cmd(module_t *module, const char *cmd_name, const char *description, cmd_callback callback);
void cmd_handler_deinit(void);
void cmd_handler_init(void);
#endif
