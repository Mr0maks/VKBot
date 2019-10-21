#pragma once

#include "vkapi.h"

/* random.c */
void cmd_rand(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_date(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_who(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_flip(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_info(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rate(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_choose(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_rand_docs(vkapi_message_object *message, int argc, char **argv, const char *args);

/* utils.c */
void cmd_ping(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_valute_curse(vkapi_message_object *message, int argc, char **argv, const char *args );
void cmd_weather(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_crc32(vkapi_message_object *message, int argc, char **argv, const char *args);

/* fun.c */
void cmd_hlmemes(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_cat(vkapi_message_object *message, int argc, char **argv, const char *args);

/* chat.c */
void cmd_reload(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_profile(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_set_greeting(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_set_privilage(vkapi_message_object *message, int argc, char **argv, const char *args);

/* debug.c */
void cmd_debug(vkapi_message_object *message, int argc, char **argv, const char *args);

/* vk.c */
void cmd_start(vkapi_message_object *message, int argc, char **argv, const char *args);
