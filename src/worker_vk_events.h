#pragma once

#include "vkapi.h"

#ifdef cJSON__h
typedef bool (*event_handler_t)(cJSON *ptr);
typedef int (*event_hook_handler_t)(cJSON *ptr);
#else
typedef bool (*event_handler_t)(void *ptr);
typedef int (*event_hook_handler_t)(void *ptr);
#endif

enum
{
    MODULE_IGNORE,
    MODULE_OVERRIDE,
    MODULE_ERROR
};

typedef struct event_s
{
    const char *event_name;
    event_handler_t handler;
    struct event_s *next;
} event_t;

typedef struct event_hook_s
{
    const char *event_name;
    event_hook_handler_t handler;
    struct event_hook_s *next;
} event_hook_t;

#ifndef _VKBOT_MODULE
void module_event_register(const char *event_name, event_handler_t handler);
void module_event_hook_register(const char *event_name, event_hook_handler_t handler);
event_handler_t event_find(const char *event_name);
bool events_manager(cJSON *raw);
#endif
