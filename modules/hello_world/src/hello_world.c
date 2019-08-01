#include <stdio.h>
#include <string.h>
#include <module_api.h>

engine_api_t engine_api;

void cmd_hello_world(vkapi_handle *obj, vkapi_message_object *message, int argc, char *argv[], const char *args )
{
  (*engine_api.vkapi_send_message)(obj, message->peer_id, "Hello from module world!");
}

void Module_Init(engine_api_t *api, int ver)
{
  memcpy(&engine_api, api, sizeof(engine_api));

  (*engine_api.register_command)("helloworld", "hello world модуль для демонстрации", cmd_hello_world);
}
