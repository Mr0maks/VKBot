#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

#include "cmds.h"

engine_api_t engine_api;

module_info_t module_info =
{
    "openssl",
    "0.1",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

const cmds_t commands[] = {
  { "sha1", "подсчитывает sha1 хеш строки или файла", cmd_sha1 },
  { "sha256", "подсчитывает sha256 хеш строки или файла", cmd_sha256 },
  { "md5", "подсчитывает md5 хеш строки или файла", cmd_md5 },
  { NULL, NULL, NULL }
};

void Module_Init_Cmds()
{
    for(int i = 0; commands[i].string != NULL; i++)
    REGISTER_COMMAND(commands[i].string, commands[i].description, commands[i].function);
}

module_info_t Module_Init(int apiver, engine_api_t *apifuncs)
{
    memcpy(&engine_api, apifuncs, sizeof(engine_api));

    Module_Init_Cmds();
    return module_info;
}
