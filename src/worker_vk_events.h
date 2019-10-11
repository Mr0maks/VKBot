#pragma once

#include "vkapi.h"

#ifdef cJSON__h
typedef bool (*event_handler_t)(vkapi_handle *handle, cJSON *ptr);
#else
typedef bool (*event_handler_t)(vkapi_handle *handle, void *ptr);
#endif

typedef struct event_s
{
    const char *event_name;
    event_handler_t handler;
    struct event_s *next;
} event_t;

#ifndef _VKBOT_MODULE
void module_event_register(const char *event_name, event_handler_t handler);
event_handler_t event_find(const char *event_name);
bool events_manager(vkapi_handle *handle, cJSON *raw);
#endif
