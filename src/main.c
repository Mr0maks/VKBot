#include <string.h>
#include "sys_con.h"
#include "config.h"
// void

void worker_main_thread( const char *token, int num_workers );

int main(int argc, char *argv[])
{
    if(config_parse_file("./bot.ini"))
    {
        Con_Printf("Bot config parsing error");
        return 0;
    }

  (void)argc;
  (void)argv;

  worker_main_thread( config.token, config.num_workers );

  return 0;
}
