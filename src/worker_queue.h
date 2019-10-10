#pragma once

#include "vkapi.h"

int queue_empty(void);
size_t queue_maxium_tasks(void);
void queue_push(cJSON *x);
cJSON *queue_pop(void);
void queue_deinit(void);
void queue_init(void);
