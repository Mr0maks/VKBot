#pragma once

#include "vkapi.h"

void cmd_help(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_ping(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_base64_encode(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_base64_decode(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_stat(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_rand(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_rand_date(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_flip(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_info(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
void cmd_debug(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args);
