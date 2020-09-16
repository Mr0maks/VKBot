/*
events_handler.c - Bot VK events handler
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "common.h"
#include "events_handler.h"
#include "engine_events.h"
#include "module_events.h"

typedef struct event_s
{
    const char *event_name;
    event_handler_t handler;
    struct event_s *next;
} event_t;

typedef struct event_hook_s
{
    const char *event_name;
    event_handler_t handler;
    struct event_hook_s *next;
} event_hook_t;

static event_t *module_pool = NULL;
static event_hook_t *module_hooks_pool = NULL;

static event_t default_events[] = 
{
    { "message_new", message_new_handler, NULL },
	{ NULL, NULL, NULL }
};

void module_event(module_t *module, const char *event_name, event_handler_t handler)
{
    assert(event_name);
    assert(handler);

    event_t *ptr = malloc(sizeof (event_t));

    ptr->event_name = event_name;
    ptr->handler = handler;

    ptr->next = module_pool;
    module_pool = ptr;

    Con_Printf("[Event] Register '%s'\n", event_name);
}

void module_event_hook(module_t *module, const char *event_name, event_handler_t handler)
{
    assert(event_name);
    assert(handler);

    event_hook_t *ptr = malloc(sizeof (event_hook_t));

    ptr->event_name = event_name;
    ptr->handler = handler;

    ptr->next = module_hooks_pool;
    module_hooks_pool = ptr;

    Con_Printf("[Event] Register hook for '%s'\n", event_name);
}

int event_proceed_hooks(const char *event_name, cJSON *obj)
{
    int result = MODULE_IGNORE;

    event_hook_t *ptr = module_hooks_pool;

    while (ptr) {

        if(!strncrc32cmp(event_name, ptr->event_name, strlen(event_name)))
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
    assert(raw);
    
    cJSON *type = cJSON_GetObjectItem(raw, "type");
    cJSON *object = cJSON_GetObjectItem(raw, "object");
    cJSON *message = cJSON_GetObjectItem(object, "message");
    cJSON *action = cJSON_GetObjectItem(message, "action");
    cJSON *action_type = cJSON_GetObjectItem(action, "type");

    if(!type)
    {
        return false;
    }

    if(action_type)
    {
        type = action_type;
    }

    // proceed actions like longpool events
    
    event_handler_t handler = event_find(cJSON_GetStringValue(type));
    
    int overwrite = event_proceed_hooks(cJSON_GetStringValue(type), raw);

    if(overwrite == MODULE_OVERWRITE)
        return true;

    if(!handler)
        return false;
    
    return handler(raw);
}
