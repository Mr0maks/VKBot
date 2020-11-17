#include <cJSON.h>
#include <enginecallbacks.h>
#include <strings.h>

#if 0
int message_new_hook(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(message, "from_id");
    cJSON *text = cJSON_GetObjectItem(message, "text");

    if(!from_id || !peer_id || !text)
        return false;

    return MODULE_IGNORE;
}
#endif

int chat_invite_user_handler(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *action = cJSON_GetObjectItem(message, "action");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *member_id = cJSON_GetObjectItem(action, "member_id");

    if(!member_id || !peer_id)
        return false;

    if(member_id->valueint < 0)
    {
            VKAPI_SEND_MESSAGE(peer_id->valueint, "Привет бот >:(", NULL, 0 );
    } else {
    VKAPI_SEND_MESSAGE(peer_id->valueint, "Приветствую в беседе!", NULL, 0 );
    }
    return true;
}

int chat_kick_user(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *action = cJSON_GetObjectItem(message, "action");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *member_id = cJSON_GetObjectItem(action, "member_id");

    if(!member_id || !peer_id)
        return false;

    if(member_id->valueint < 0)
    {
        VKAPI_SEND_MESSAGE(peer_id->valueint, "Пока бот :)", NULL, 0 );
    } else {
        VKAPI_SEND_MESSAGE(peer_id->valueint, "Этот парень был из тех, кто просто любит жизнь.", NULL, 0 );
    }
    return true;
}
