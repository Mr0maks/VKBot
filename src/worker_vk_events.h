#pragma once

#include "vkapi.h"

#ifdef cJSON__h
typedef int (*event_handler_t)(cJSON *ptr);
#else
typedef int (*event_handler_t)(void *ptr);
#endif

#ifndef _VKBOT_MODULE
event_handler_t event_find(const char *event_name);
bool events_manager(cJSON *raw);
#endif
