/*
config.c - Bot configuration
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
        config.debug = true;
        Sys_Reg_LogFile();
        return 0;
    }

    return 1;
}

int config_parse_file(const char *filename)
{
    if(!filename)
        return 1;

    if (mini_ini_parse(filename, config_handler, NULL)) {
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
