#pragma once

#pragma once

#include "vkapi.h"

/* sha.c */
void cmd_sha1(vkapi_message_object *message, int argc, char **argv, const char *args);
void cmd_sha256(vkapi_message_object *message, int argc, char **argv, const char *args);

/* md5.c */
void cmd_md5(vkapi_message_object *message, int argc, char **argv, const char *args);
