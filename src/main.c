#include <string.h>
#include "sys_con.h"
#include "config.h"
// void

void Host_Init(void);

int main(int argc, char *argv[])
{
    if(config_parse_file("./bot.ini"))
    {
        Con_Printf("Bot config parsing error");
        return 0;
    }

    (void)argc;
    (void)argv;

    Host_Init();
    return 0;
}
