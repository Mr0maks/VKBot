#pragma once

typedef int (*mini_ini_handler_t)(const char *section, const char *key, const char *value, void *userdata);

int mini_json_parse(const char *filename, mini_ini_handler_t handler, void *userdata );
int mini_json_tokeinize( unsigned char *str, mini_ini_handler_t handler, void *dataptr );
