#pragma once

#include <cJSON.h>

int queue_is_empty();
void add_queue(cJSON *x);
void queue_init();
cJSON *get_queue();
