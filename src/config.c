#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include "minini.h"
#include "config.h"

bot_config_t config = { 0 };

static int config_handler(const char *section, const char *key, const char *value, void* user)
{
    if(!strncasecmp("token", key, 5))
    {
        config.token = strdup(value);
        return 0;
    } else if (!strncasecmp("num_workers", key, 7))
    {
        config.num_workers = atoi(value);
        return 0;
    } else if(!strncasecmp("debug", key, 5))
    {
        Sys_Reg_LogFile();
        return 0;
    } else if(!strncasecmp("debug_workers", key, 5))
    {
        if(!strcasecmp("true", value))
            config.debug_workers = true;
        else
            config.debug_workers = false;

        return 0;
    }

    return 1;
}

int config_parse_file(const char *filename)
{
    if(!filename)
        return 1;

    if (minini_parse(filename, config_handler, NULL)) {
        Con_Printf("Can't load '%s'\n", filename);
        return 1;
    }

    Con_Printf("Config loaded from '%s'\n", filename);
    return 0;
}

void config_deinit()
{
    if(config.token)
    free(config.token);
}
