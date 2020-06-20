#include "common.h"
#include "cmd_handler.h"
#include "crc32.h"
#include "module_api.h"
#include "minini.h"

#include <stdarg.h>
#include <string.h>

module_t *modules_pool = NULL;

typedef module_info_t (*module_init)( int apiver, engine_api_t *apifuncs );

void cmd_handler_register_module_cmd(module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback);

void module_reg_cmd(module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback)
{
  cmd_handler_register_module_cmd(info, cmd_name, description, callback);
}

const engine_api_t engfuncs_t =
{
    // memory api
    NULL,
    NULL,

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
    curl_postfield_serialize,
    curl_postfield_destroy,
    curl_get,
    curl_post,
    curl_uploadfile,
    curl_cleanup,

    //events
    module_event_register,
    module_event_hook_register,

    //module api
    module_load,
    module_loaded,
    module_function,

    // other stuff
    memcrc32,
    Con_Printf,
};

static void module_fill_funcs(engine_api_t *ptr)
{
    memcpy(ptr, &engfuncs_t, sizeof (engfuncs_t));
}

bool module_load(const char *name)
{
    assert(name);

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

  module_fill_funcs(&ptr->local_copy);

  module_info_t info = module_init_func(ENGINE_API_VERSION, &ptr->local_copy );

  ptr->info.name = strdup(info.name);
  ptr->info.version = strdup(info.version);
  ptr->info.date = strdup(info.date);
  ptr->info.url = strdup(info.url);
  ptr->info.author = strdup(info.author);
  ptr->info.ifver = info.ifver;

  if(ptr->info.ifver != ENGINE_API_VERSION)
  {
      Con_Printf("Module %s error: interface mismatch\n", name);
      FreeLibrary(ptr->handle);
      free(ptr);
      return false;
  }

  strncpy(ptr->name, name, sizeof(ptr->name));

  ptr->next = modules_pool;
  modules_pool = ptr;
  return true;
}

bool module_loaded(const char *name)
{
    assert(name);

    if(name == NULL)
        return false;

    module_t *ptr = modules_pool;

    while(ptr != NULL)
    {
        if(!strcmp(name, ptr->name))
        {
            return true;
        }
        ptr = ptr->next;
    }

    return false;
}

void *module_function(const char *name, const char *function)
{
    assert(name);
    assert(function);

    if(name == NULL || function == NULL)
        return NULL;

    module_t *ptr = modules_pool;

    while(ptr != NULL)
    {
        if(!strcmp(name, ptr->name))
        {
            return GetProcAddress(ptr->handle, function);
        }
        ptr = ptr->next;
    }

    return NULL;
}

static int module_minini(const char *section, const char *key, const char *value, void* user)
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
