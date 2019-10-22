#include <enginecallbacks.h>
#include <cJSON.h>
#include <strings.h>
#include "va_utils.h"

bool vkapi_is_chat_admin(int peer_id, int id)
{
    string_t format = STRING_INIT();

    STRING_FORMAT(format, "peer_id=%i", peer_id);

    string_t result = VKAPI_CALL_METHOD("messages.getConversationMembers", format, true);

    STRING_DESTROY(format);

    cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

    if(!ptr)
        return false;

    cJSON *element = NULL;

    cJSON_ArrayForEach(element, cJSON_GetObjectItem(cJSON_GetObjectItem(ptr, "response"), "items"))
    {
        if(cJSON_GetObjectItem(element, "member_id")->valueint == id)
        {
            cJSON *is_admin = cJSON_GetObjectItem(element, "is_admin");
            if(!is_admin)
                goto admin_false;

        if(is_admin->valueint)
            goto admin_true;
        else
            goto admin_false;
        }
    }

    admin_false:
    STRING_DESTROY(result);
    cJSON_Delete(ptr);
    return false;

    admin_true:
    STRING_DESTROY(result);
    cJSON_Delete(ptr);
    return true;
}
