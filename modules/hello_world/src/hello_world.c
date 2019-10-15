#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

engine_api_t engine_api;

void cmd_hello_world(vkapi_message_object *message, int argc, char **argv, const char *args )
{
  VKAPI_SEND_MESSAGE(message->peer_id, "Hello from module world!", NULL, 0);
}

module_info_t module_info =
{
    "hello world",
    "0",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  REGISTER_COMMAND(&module_info, "helloworld", "hello world модуль для демонстрации", cmd_hello_world);
}
