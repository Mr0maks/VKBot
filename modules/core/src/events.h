#pragma once

#include <enginecallbacks.h>

/*int message_new_hook(cJSON *raw);*/
int chat_invite_user_handler(cJSON *raw);
int chat_kick_user(cJSON *raw);
