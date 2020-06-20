#include <string.h>
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

module_info_t Module_Init(int apiver, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));

  Module_Init_Dbs();
  return module_info;
}
