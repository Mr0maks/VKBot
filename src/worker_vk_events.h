#pragma once

#include "vkapi.h"

typedef vkapi_boolean (*event_handler_t)(vkapi_handle *handle, cJSON *ptr);

typedef struct event_s
{
    const char *event_name;
    event_handler_t handler;
    struct event_s *next;
} event_t;

event_handler_t event_find(const char *event_name);
vkapi_boolean events_manager(vkapi_handle *handle, cJSON *raw);
