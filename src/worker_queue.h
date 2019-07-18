#pragma once

#include "vkapi.h"

int queue_is_empty();
void add_queue(cJSON *x);
void queue_init();
vkapi_message_new_object *get_queue();
