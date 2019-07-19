#pragma once

#include "vkapi.h"

int queue_is_empty(void);
size_t quequ_maxium_tasks_in_quequ(void);
void add_queue(cJSON *x);
void queue_init(void);
vkapi_message_new_object *get_queue(void);
