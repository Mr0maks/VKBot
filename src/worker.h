#pragma once

#include "vkapi.h"

#ifndef _VKBOT_DEBUG
int worker_get_worker_id(void);
vkapi_handle *worker_get_vkapi_handle(void);
int worker_get_workers_count(void);
size_t worker_commands_processed(void);
size_t worker_message_processed(void);
#endif
