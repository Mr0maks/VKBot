#include <cJSON.h>
#include <enginecallbacks.h>
#include <strings.h>

int message_new_hook(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(message, "from_id");
    cJSON *text = cJSON_GetObjectItem(message, "text");

    if(!from_id || !peer_id || !text)
        return false;

    if(0 > from_id->valueint && !strcasecmp(cJSON_GetStringValue(text), "саксбот"))
    {
        VKAPI_SEND_MESSAGE(peer_id->valueint, "Привет бот >:(", NULL, 0 );
        VKAPI_SEND_MESSAGE(peer_id->valueint, "максбот", NULL, 0 );
    }

    return MODULE_IGNORE;
}

bool chat_invite_user_handler(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(message, "from_id");

    if(!from_id || !peer_id)
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
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *peer_id = cJSON_GetObjectItem(message, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(message, "from_id");

    if(!from_id || !peer_id)
        return false;

    if(from_id->valueint < 0)
    {
        VKAPI_SEND_MESSAGE(peer_id->valueint, "Пока бот :)", NULL, 0 );
    } else {
        VKAPI_SEND_MESSAGE(peer_id->valueint, "Этот парень был из тех, кто просто любит жизнь.", NULL, 0 );
    }
    return false;
}
