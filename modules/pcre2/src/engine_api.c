#include <module_api.h>
#include <enginecallbacks.h>
#include "cmd.h"

engine_api_t engine_api;

module_info_t module_info =
{
    "pcre2",
    "0.1",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

const cmds_t commands[] = {
  { "pcre2", "доступ к pcre2", cmd_pcre },
#ifdef DEBUG
#endif
  { NULL, NULL, NULL }
};

void Module_Init_Cmds()
{
    for(int i = 0; commands[i].string != NULL; i++)
    REGISTER_COMMAND(&module_info, commands[i].string, commands[i].description, commands[i].function);
}

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  Module_Init_Cmds();
}
