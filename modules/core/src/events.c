#include <cJSON.h>
#include <enginecallbacks.h>
#include "db_core.h"

bool chat_invite_user_handler(vkapi_handle *handle, cJSON *raw)
{
    cJSON *peer_id = cJSON_GetObjectItem(cJSON_GetObjectItem(raw, "object"), "peer_id");

    if(!peer_id)
        return false;

    string_t greetings = db_chat_greetings_get(peer_id->valueint);

    if(greetings)
    {
        VKAPI_SEND_MESSAGE(handle, peer_id->valueint, greetings->ptr, NULL, 0 );
        STRING_DESTROY(greetings);
        return false;
    }

    VKAPI_SEND_MESSAGE(handle, peer_id->valueint, "Приветствую в беседе!", NULL, 0 );
    return true;
}

bool chat_kick_user(vkapi_handle *handle, cJSON *raw)
{
    return false;
}
