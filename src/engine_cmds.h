#pragma once

#include "vkapi.h"

void cmd_help(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_modules(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_about_bot(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_stat(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_gc(vkapi_message_object *message, int argc, char **argv, const char *args);
