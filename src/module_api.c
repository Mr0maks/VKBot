#include <stdio.h>
#include <memory.h>
#include "va_utils.h"
#include "cmd_handler.h"
#include "crc_hash.h"
#include "module_api.h"
#include "memcache.h"

module_t *modules_pool = NULL;

typedef void (*module_init)( int apiver, module_info_t **info, engine_api_t *apifuncs );

void cmd_handler_register_module_cmd(module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback);

void module_reg_cmd(module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback)
{
  cmd_handler_register_module_cmd(info, cmd_name, description, callback);
}

void module_alert(module_info_t *info, const char *fmt, ...)
{
    printf("[%s] STUB!\n", info->name );
}

const engine_api_t engfuncs_t =
{
    module_reg_cmd,
    NULL, // FIXME: unregister not implemented

    // vkapi
    vkapi_call_method,
    vkapi_send_message,
    vkapi_upload_doc_by_url,

    // string api
    string_init,
    string_dublicate,
    string_format,
    _string_strncat_char,
    string_copy,
    string_destroy,

    // curl
    curl_init,
    curl_get,
    curl_post,
    curl_uploadfile,
    curl_cleanup,

    // memcache
    memcache_push,
    memcache_get,

    // db api
    db_register_type,
    db_open,
    db_exec,
    db_close,

    // users api
    users_is_init,
    users_module_register_users,
    users_get_privilage,
    users_get_name_privilage,

    // other shit
    crc32_calc,
    module_alert,
};

void module_fill_funcs(engine_api_t *ptr)
{
    memcpy(ptr, &engfuncs_t, sizeof (engfuncs_t));
}

void module_load(const char *name)
{
  if(!name)
    return;

  module_t *ptr = (module_t*)calloc(1, sizeof(module_t));

  if(!ptr)
    {
      printf("Module %s error: calloc return NULL\n", name);
      return;
    }

  ptr->handle = LoadLibrary(va("./%s.%s", name, LIB_EXT));

  if(ptr->handle == NULL)
    {
      printf("Module %s error: %s\n", name, GetLastError());
      free(ptr);
      return;
    }

  module_init module_init_func = (module_init)dlsym(ptr->handle, "Module_Init");

  if(!module_init_func)
    {
      printf("Module %s error: %s\n", name, GetLastError());
      free(ptr);
      return;
    }

  engine_api_t local_copy;

  module_fill_funcs(&local_copy);

  module_init_func(ENGINE_API_VERSION, &ptr->info, &local_copy );

  if(ptr->info->ifver != ENGINE_API_VERSION)
  {
      printf("Module %s error: interface mismatch\n", name);
      FreeLibrary(ptr->handle);
      free(ptr);
      return;
  }

  strncpy(ptr->name, name, sizeof(ptr->name));

  ptr->next = modules_pool;
  modules_pool = ptr;
}

void load_modules() {
  module_load("sqlite");
  module_load("users_sqlite");
  module_load("core");
//  module_load("helloworld");
//  module_load("bytecode");
}
