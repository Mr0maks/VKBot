#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>
#include "sqlite3_db.h"

engine_api_t engine_api;

module_info_t module_info =
{
    "sqlite",
    "0.1",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};


void Module_Init_Dbs()
{
  DB_REGISTER("sqlite", sqlite_open, sqlite_exec, sqlite_close, NULL );
}

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  Module_Init_Dbs();
}
