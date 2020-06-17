#include "common.h"
#include "config.h"

static volatile bool main_thread_loop = false;

void Host_Deinit()
{
    worker_deinit();
    cmd_handler_deinit();
    curl_worker_share_deinit();
}

void Host_Main_Thread()
{
    while (main_thread_loop) {
        sleep(1000);
    }

    Host_Deinit();
}

void Host_Exit()
{
    main_thread_loop = false;
}

void load_modules(void);

void Host_Init()
{
    if(config_parse_file("./bot.ini"))
    {
        Con_Printf("Bot config parsing error");
        return;
    }

    main_thread_loop = true;

    load_modules();

    cmd_handler_init();
    curl_worker_share_init();
    worker_init();

    Host_Main_Thread();
}

int main(void)
{
    Host_Init();
    return 0;
}
