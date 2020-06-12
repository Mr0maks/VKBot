#pragma once

#include "vkapi.h"

/* random.c */
void cmd_rand(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_date(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_who(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_flip(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_info(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rate(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_docs(vkapi_message_object *message, int argc, char **argv, const char *args);

/* utils.c */
void cmd_ping(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_valute_course(vkapi_message_object *message, int argc, char **argv, const char *args );
void cmd_weather(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_crc32(vkapi_message_object *message, int argc, char **argv, const char *args);

/* fun.c */
void cmd_cat(vkapi_message_object *message, int argc, char **argv, const char *args);

/* debug.c */
void cmd_debug(vkapi_message_object *message, int argc, char **argv, const char *args);

/* vk.c */
void cmd_start(vkapi_message_object *message, int argc, char **argv, const char *args);
