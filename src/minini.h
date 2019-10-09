#pragma once

typedef int (*mini_ini_handler_t)(const char *section, const char *key, const char *value, void *userdata);

int minini_parse(const char *filename, mini_ini_handler_t handler, void *userdata );
int minini_tokeinize( unsigned char *str, mini_ini_handler_t handler, void *dataptr );
