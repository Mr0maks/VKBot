#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>
#include "users.h"

engine_api_t engine_api;

module_info_t module_info =
  {
    "users_nodb",
    "0.1",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

users_t users = {
  NULL,
  get_privilage_by_id,
  get_name_privilage,
};

void Module_Init_Users()
{
  USERS_REGISTER(&users);
}

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  Module_Init_Users();
}
