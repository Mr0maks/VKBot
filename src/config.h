#pragma once

typedef struct config_s
{
    const char *token;
    int num_workers;
} bot_config_t;

extern bot_config_t config;

int config_parse_file(const char *filename);
void config_deinit();
