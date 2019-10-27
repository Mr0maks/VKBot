#pragma once

#include "common.h"

typedef struct config_s
{
    char *token;
    int num_workers;
    bool debug_workers;
    bool gc_disable;
} bot_config_t;

extern bot_config_t config;

int config_parse_file(const char *filename);
void config_deinit();
