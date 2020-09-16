/*
module_api.c - modules for bot
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
#include "module_api.h"
#include "minini.h"
#include "module.h"
#include <string.h>

module_t *modules_pool = NULL;
module_t *module_loading = NULL;

typedef module_info_t (*module_init)( int apiver, engine_api_t *apifuncs );

void module_cmd(module_t *module, const char *cmd_name, const char *description, cmd_callback callback);

void module_register_cmd(const char *cmd_name, const char *description, cmd_callback callback)
{
    assert(module_loading);
    module_cmd(module_loading, cmd_name, description, callback);
}

void module_register_event(const char *event_name, event_handler_t handler)
{
    assert(module_loading);
    module_event(module_loading, event_name, handler);
}

void module_register_event_hook(const char *event_name, event_handler_t handler)
{
    assert(module_loading);
    module_event_hook(module_loading, event_name, handler);
}

engine_api_t engine_functions =
{
    // memory api
    malloc,
    free,

    module_register_cmd,
    module_register_event,
    module_register_event_hook,

    // vkapi
    vkapi_call_method,
    vkapi_send_message,
    vkapi_upload_attachment,

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

    //module api
    module_load,
    module_loaded,
    module_function,

    // other stuff
    memcrc32,
    Con_Printf,
};

bool module_load(const char *name)
{
    assert(name);
    assert(module_loading == NULL);

  module_t *ptr = (module_t*)calloc(1, sizeof(module_t));
  module_loading = ptr;

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

  strncpy(ptr->name, name, sizeof(ptr->name));
  module_info_t info = module_init_func(ENGINE_API_VERSION, &engine_functions);

  ptr->info.name = info.name;
  ptr->info.version = info.version;
  ptr->info.date = info.date;
  ptr->info.url = info.url;
  ptr->info.author = info.author;
  ptr->info.ifver = info.ifver;

  if(ptr->info.ifver != ENGINE_API_VERSION)
  {
      Con_Printf("Module %s error: interface version mismatch\nEngine: %d\nModule: %d\n", name, ENGINE_API_VERSION, ptr->info.ifver);
      FreeLibrary(ptr->handle);
      free(ptr);
      return false;
  }

  ptr->next = modules_pool;
  modules_pool = ptr;
  module_loading = NULL;
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

void *module_unload(const char *name)
{
    assert(name);

    if(name == NULL)
        return NULL;

    module_t *ptr = modules_pool, *prev = NULL;

    while(ptr != NULL)
    {
        if(!strcmp(name, ptr->name))
        {
            prev->next = ptr->next;
        }
        ptr = ptr->next;
        prev = ptr;
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
    if(mini_ini_parse("./modules.ini", module_minini, NULL ))
    {
        Con_Printf("[Module] Cant load modules: parse error\n");
        return;
    }
}
