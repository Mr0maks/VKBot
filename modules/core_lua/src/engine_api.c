#include <cJSON.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

engine_api_t engine_api;

module_info_t module_info =
{
    "core_lua",
    "0.1",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

int Lua_Init(void);

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  Lua_Init();

}
