#include <stdio.h>
#include <memory.h>
#include "va_utils.h"
#include "cmd_handler.h"
#include "crc_hash.h"
#include "module_api.h"

#ifdef _WIN32
#include <Windows.h>
#define LIB_EXT "dll"
#else
#include <dlfcn.h>
#define LoadLibrary( x ) dlopen( x, RTLD_NOW )
#define GetProcAddress( x, functionname ) dlsym( x , functionname )
#define FreeLibrary( x ) dlclose( x )
#define GetLastError( ) dlerror( )
typedef void* HMODULE ;
#define LIB_EXT "so"
#endif

#define ENGINE_API_VER 0

static int module_id_has_loading = 0;
static int modules_loaded = 0;

typedef struct module_s
{
  HMODULE handle;
  int	  module_id;
  char	  name[256];
  struct module_s *next;
} module_t;

module_t *modules_pool = NULL;

typedef void (*module_init)(engine_api_t *apifuncs, int apiver );

void cmd_handler_register_module_cmd(int module_id, const char *cmd_name, const char *description, cmd_function_callback callback);

void module_reg_cmd(const char *cmd_name, const char *description, cmd_function_callback callback)
{
  cmd_handler_register_module_cmd(module_id_has_loading, cmd_name, description, callback);
}

engine_api_t engine_funcs = {
  module_reg_cmd,
  vkapi_send_message,
  string_init,
  string_dublicate,
  string_format,
  _string_strncat_char,
  string_copy,
  string_destroy,
  crc32_calc
};

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

  module_init funct_ptr = (module_init)dlsym(ptr->handle, "Module_Init");

  if(!funct_ptr)
    {
      printf("Module %s error: %s\n", name, GetLastError());
      free(ptr);
      return;
    }

  engine_api_t local_copy;

  module_id_has_loading++;

  memcpy(&local_copy, &engine_funcs, sizeof(engine_api_t));

  funct_ptr(&local_copy, ENGINE_API_VER);

  strncpy(ptr->name, name, sizeof(ptr->name));

  ptr->module_id = module_id_has_loading;

  ptr->next = modules_pool;
  modules_pool = ptr;
}

void load_modules() {
  module_load("helloworld");
  module_load("bytecode");
}
