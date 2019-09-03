#pragma once

#include "vkapi.h"

void cmd_help(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_modules(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_about_bot(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_stat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_gc(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
