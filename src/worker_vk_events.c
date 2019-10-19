#include "common.h"
#include "worker_vk_events.h"

static event_t *module_pool = NULL;
static event_hook_t *module_hooks_pool = NULL;

bool message_new_handler(cJSON *raw);

static event_t default_events[] = 
{
    { "message_new", message_new_handler, NULL },
    { NULL, NULL, NULL, }
};

void module_event_register(const char *event_name, event_handler_t handler)
{
    if(!event_name || !handler)
        return;

    event_t *ptr = malloc(sizeof (event_t));

    ptr->event_name = event_name;
    ptr->handler = handler;

    ptr->next = module_pool;
    module_pool = ptr;

    Con_Printf("[Event] Register '%s'\n", event_name);
}

void module_event_hook_register(const char *event_name, event_hook_handler_t handler)
{
    if(!event_name || !handler)
        return;

    event_hook_t *ptr = malloc(sizeof (event_hook_t));

    ptr->event_name = event_name;
    ptr->handler = handler;

    ptr->next = module_hooks_pool;
    module_hooks_pool = ptr;

    Con_Printf("[Event] Register hook for '%s'\n", event_name);
}

int event_proced_hooks(const char *event_name, cJSON *obj)
{
    int result = MODULE_IGNORE;

    event_hook_t *ptr = module_hooks_pool;

    while (ptr) {

        if(!strcrc32casecmp(event_name, ptr->event_name))
        {
            result = ptr->handler(obj);
        }

        ptr = ptr->next;
    }

    return result;
}

event_handler_t event_find(const char *event_name)
{
    event_t *ptr = module_pool;

    while (ptr != NULL) {
        if(!strncasecmp(event_name, ptr->event_name, strlen(event_name)))
            return ptr->handler;

        ptr = ptr->next;
    }

    for(int i = 0; default_events[i].event_name != NULL; i++ )
    {
        if(!strncasecmp(event_name, default_events[i].event_name, strlen(event_name)))
            return default_events[i].handler;
    }
    
    Con_Printf("Warn: Event '%s' not register in event system!\n", event_name);

    return NULL;
}

bool events_manager(cJSON *raw)
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
    
    int override = event_proced_hooks(cJSON_GetStringValue(type), raw);

    if(override == MODULE_OVERRIDE)
        return true;

    if(!handler)
        return false;
    
    return handler(raw);
}
