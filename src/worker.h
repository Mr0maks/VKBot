#pragma once

#include <stdlib.h>

#ifndef _VKBOT_DEBUG
int worker_get_workers_count(void);
size_t worker_commands_processed(void);
size_t worker_message_processed(void);
#endif
