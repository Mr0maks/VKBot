/*
host.c - Bot initialization/deinitialization
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "common.h"
#include "config.h"
#include <signal.h>

static volatile bool main_thread_loop = false;

void Bot_Deinit()
{
    worker_deinit();
    cmd_handler_deinit();
    curl_worker_share_deinit();
    config_deinit();
}

void longpoll_worker(void);

void Bot_Main_Thread_Exit(int i)
{
    (void)i;

    Con_Printf("Bot deinitialization started\n");

    main_thread_loop = false;
}

void load_modules(void);

static time_t init_time = 0;

time_t Bot_Init_Time(void)
{
    return init_time;
}

void Bot_Init()
{
#ifdef VKBOT_FIND_LEAK
    GC_set_find_leak(1);
    GC_INIT();
#endif
    init_time = time(NULL);

    if (config_parse_file("./bot.ini")) {
        Con_Printf("Error while parsing bot configuration");
        return;
    }

    load_modules();

    cmd_handler_init();
    curl_worker_share_init();
    worker_init();

    main_thread_loop = true;
    signal(SIGINT, Bot_Main_Thread_Exit);

    while (main_thread_loop) {
        longpoll_worker();
    }

    Bot_Deinit();
}

int main(void)
{
    Bot_Init();
    return 0;
}
