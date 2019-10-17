#pragma once

#include "vkapi.h"

void cmd_ping(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_base64_encode(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_base64_decode(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_date(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_who(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_flip(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_info(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rate(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_choose(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_docs(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_valute_curse(vkapi_message_object *message, int argc, char **argv, const char *args );
void cmd_weather(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_crc32(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_hlmemes(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_cat(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_get_privilage(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_debug(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_set_greeting(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_set_privilage(vkapi_message_object *message, int argc, char **argv, const char *args);
