#pragma once

#include "vkapi.h"

int queue_is_empty(void);
size_t queue_maxium_tasks_in_queue(void);
void add_queue(cJSON *x);
void queue_init(void);
vkapi_message_new_object *get_queue(void);
