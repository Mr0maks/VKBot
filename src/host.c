#include "common.h"

static volatile bool main_thread_loop = false;

void Host_Deinit()
{
    worker_deinit();
    cmd_handler_deinit();
    queue_deinit();
}

void Host_Main_Thread()
{
    char buff[256] = { '\0' };

    while (main_thread_loop) {
        while(fgets(buff, sizeof (buff), stdin) == NULL) { }
        console_handler(buff);
    }

    Host_Deinit();
}

void Host_Exit()
{
    main_thread_loop = false;
}

void load_modules(void);

void Host_Memmgr_Init(void);

void Host_Init()
{
    main_thread_loop = true;

    Host_Memmgr_Init();

    memcache_init(64);

    load_modules();

    queue_init();
    cmd_handler_init();
    worker_init();

    Host_Main_Thread();
}
