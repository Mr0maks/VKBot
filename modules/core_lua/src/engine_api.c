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

module_info_t Module_Init(int apiver, engine_api_t *apifuncs)
{
    memcpy(&engine_api, apifuncs, sizeof(engine_api));
    Lua_Init();
    return module_info;
}
