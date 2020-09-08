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
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(message, "from_id");
    cJSON *text = cJSON_GetObjectItem(message, "text");

    if(peer_id)
    {
	VKAPI_SEND_MESSAGE(peer_id->valueint, "Hello from hook message_new!\n", NULL, 0);
	return MODULE_OVERWRITE;
    }

    return MODULE_IGNORE;
}

module_info_t Module_Init(int apiver, engine_api_t *apifuncs)
{
    memcpy(&engine_api, apifuncs, sizeof(engine_api));
    REGISTER_EVENT_HOOK("message_new", message_new_hook );
    return module_info;
}
