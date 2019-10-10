#include "common.h"
#include "worker_vk_events.h"

static event_t *module_pool = NULL;

vkapi_boolean message_new_handler(vkapi_handle *handle, cJSON *raw);

static event_t default_events[] = 
{
    { "message_new", message_new_handler, NULL },
    { NULL, NULL, NULL, }
};

event_handler_t event_find(const char *event_name)
{
    for(int i = 0; default_events[i].event_name != NULL; i++ )
    {
        if(!strncasecmp(event_name, default_events[i].event_name, strlen(event_name)))
            return default_events[i].handler;
    }
    
    event_t *ptr = module_pool;
    
    while (ptr != NULL) {
        if(!strncasecmp(event_name, ptr->event_name, strlen(event_name)))
            return ptr->handler;

        ptr = ptr->next;
    }
    
    Con_Printf("Warn: Event '%s' not register in event system!\n", event_name);

    return NULL;
}


vkapi_boolean events_manager(vkapi_handle *handle, cJSON *raw)
{
    if(!raw)
        return false;
    
    cJSON *type = cJSON_GetObjectItem(raw, "type");
    cJSON *action_type = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(raw, "object"), "action"), "type");
    
    if(!type)
    {
            type = action_type;
            
            if(!type)
                return false;
    } else if(type && action_type)
    {
        type = action_type;
    } else if(!type && !action_type)
    {
        return false;
    }
    
    event_handler_t handler = event_find(cJSON_GetStringValue(type));
    
    if(!handler)
        return false;
    
    return handler(handle, raw);
}
