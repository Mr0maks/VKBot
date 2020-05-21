#include <cJSON.h>
#include <enginecallbacks.h>
#include "db_core.h"
#include <string.h>
#include <strings.h>

int message_new_hook(cJSON *raw)
{
    return MODULE_IGNORE;
}

bool chat_invite_user_handler(cJSON *raw)
{
    cJSON *peer_id = cJSON_GetObjectItem(cJSON_GetObjectItem(raw, "object"), "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(cJSON_GetObjectItem(raw, "object"), "from_id");

    if(!from_id)
        return false;

    if(!peer_id)
        return false;

    if(from_id->valueint < 0)
    {
            VKAPI_SEND_MESSAGE(peer_id->valueint, "Привет бот >:(", NULL, 0 );
    } else {
    VKAPI_SEND_MESSAGE(peer_id->valueint, "Приветствую в беседе!", NULL, 0 );
    }
    return true;
}

bool chat_kick_user(cJSON *raw)
{
    return false;
}
