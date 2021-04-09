#pragma once
#include "module.h"

#ifdef cJSON__h
typedef int (*event_handler_t)(cJSON* ptr);
#else
typedef int (*event_handler_t)(void* ptr);
#endif

enum {
    MODULE_IGNORE,
    MODULE_OVERWRITE,
    MODULE_ERROR
};

void module_event(module_t* module, const char* event_name, event_handler_t handler);
void module_event_hook(module_t* module, const char* event_name, event_handler_t handler);
