#include "common.h"
#include "cmd_handler.h"
#include "crc_hash.h"
#include "module_api.h"
#include "minini.h"

#include <stdarg.h>

module_t *modules_pool = NULL;

typedef void (*module_init)( int apiver, module_info_t **info, engine_api_t *apifuncs );

void cmd_handler_register_module_cmd(module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback);

void module_reg_cmd(module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback)
{
  cmd_handler_register_module_cmd(info, cmd_name, description, callback);
}

//TODO: Move from module_api
void module_alert(const char *fmt, ...)
{
    char buff[1024] = { 0 };
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    va_end(args);
    Con_Printf(buff);
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
    curl_postfield_init,
    curl_postfield_push,
    curl_postfield_destroy,
    curl_get,
    curl_post,
    curl_uploadfile,
    curl_cleanup,

    // db api
    db_register,
    db_open,
    db_exec,
    db_close,

    //events
    module_event_register,
    module_event_hook_register,

    // other shit
    memcrc32,
    module_alert,
};

void module_fill_funcs(engine_api_t *ptr)
{
    memcpy(ptr, &engfuncs_t, sizeof (engfuncs_t));
}

bool module_load(const char *name)
{
  if(!name)
    return false;

  module_t *ptr = (module_t*)calloc(1, sizeof(module_t));

  if(!ptr)
    {
      Con_Printf("Module %s error: calloc return NULL\n", name);
      return false;
    }

  ptr->handle = LoadLibrary(va("./%s_%s.%s", name, ARCH, LIB_EXT));

  if(ptr->handle == NULL)
    {
      Con_Printf("Module %s error: %s\n", name, GetLastError());
      free(ptr);
      return false;
    }

  module_init module_init_func = (module_init)dlsym(ptr->handle, "Module_Init");

  if(!module_init_func)
    {
      Con_Printf("Module %s error: %s\n", name, GetLastError());
      free(ptr);
      return false;
    }

  engine_api_t local_copy;

  module_fill_funcs(&local_copy);

  module_init_func(ENGINE_API_VERSION, &ptr->info, &local_copy );

  if(ptr->info->ifver != ENGINE_API_VERSION)
  {
      Con_Printf("Module %s error: interface mismatch\n", name);
      FreeLibrary(ptr->handle);
      free(ptr);
      return false;
  }

  strncpy(ptr->name, name, sizeof(ptr->name));

  ptr->next = modules_pool;
  modules_pool = ptr;
}

bool module_get_func(const char *name, const char *func)
{

}

int module_minini(const char *section, const char *key, const char *value, void* user)
{
    if(value)
        return 1;

    if(section)
        return 1;

    module_load(key);

    return 0;
}

void load_modules() {
    if(mini_json_parse("./modules.ini", module_minini, NULL ))
    {
        Con_Printf("[Module] Cant load modules: parse error\n");
        return;
    }
}
