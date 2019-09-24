#pragma once

#include "vkapi.h"

void cmd_help(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_about_bot(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_ping(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_base64_encode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_base64_decode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_stat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_date(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_who(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_flip(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_info(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rate(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_docs(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_valute_curse(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args );
void cmd_weather(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_crc32(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_hlmemes(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_cat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_debug(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args);
