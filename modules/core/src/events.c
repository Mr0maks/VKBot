#include <cJSON.h>
#include <enginecallbacks.h>
#include "db_core.h"
#include <string.h>
#include <strings.h>

int message_new_hook(cJSON *raw)
{
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *peer_id = cJSON_GetObjectItem(object, "peer_id");
    cJSON *from_id = cJSON_GetObjectItem(object, "from_id");
    cJSON *attachments = cJSON_GetObjectItem(object, "attachments");

    cJSON *attach = NULL;

    if(!attachments)
        return MODULE_IGNORE;

    if(!cJSON_GetArraySize(attachments))
        return MODULE_IGNORE;

    cJSON_ArrayForEach(attach, attachments)
    {
        if(!strcmp("audio_message", cJSON_GetObjectItem(attach, "type")->valuestring))
        {
            string_t message_format = STRING_INIT();
            STRING_FORMAT(message_format, "random_id=0&message=argv[1] ненужон. [id%i|Войсер] обнаружен!\n&forward_messages=%i&peer_id=%i", from_id->valueint, cJSON_GetObjectItem(object, "conversation_message_id")->valueint, peer_id->valueint );
            VKAPI_CALL_METHOD("messages.send", message_format, false);
            STRING_DESTROY(message_format);
            return MODULE_OVERRIDE;
        }
    }

    // "conversation_message_id":540135
    return MODULE_IGNORE;
}

bool chat_invite_user_handler(cJSON *raw)
{
    cJSON *peer_id = cJSON_GetObjectItem(cJSON_GetObjectItem(raw, "object"), "peer_id");

    if(!peer_id)
        return false;

    string_t greetings = db_chat_greetings_get(peer_id->valueint);

    if(greetings)
    {
        VKAPI_SEND_MESSAGE(peer_id->valueint, greetings->ptr, NULL, 0 );
        STRING_DESTROY(greetings);
        return false;
    }

    VKAPI_SEND_MESSAGE(peer_id->valueint, "Приветствую в беседе!", NULL, 0 );
    return true;
}

bool chat_kick_user(cJSON *raw)
{
    return false;
}
