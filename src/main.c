#include "config.h"

void Host_Init(void);

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

    if(config_parse_file("./bot.ini"))
    {
        Con_Printf("Bot config parsing error");
        return 0;
    }

    Host_Init();
    return 0;
}
