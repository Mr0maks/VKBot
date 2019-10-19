#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>
#include <cJSON.h>

engine_api_t engine_api;

module_info_t module_info =
{
    "hello world hook",
    "0",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

int message_new_hook(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *peer_id = cJSON_GetObjectItem(object, "peer_id");

    if(peer_id) VKAPI_SEND_MESSAGE(peer_id->valueint, "Hello from hook message_new!\n", NULL, 0);
    if(peer_id) return MODULE_OVERRIDE;

    return MODULE_IGNORE;
}

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  REGISTER_EVENT_HOOK("message_new", message_new_hook );
}
