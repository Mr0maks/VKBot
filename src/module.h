#pragma once
#include "module_cmds.h"

typedef struct
{
    char *name;
    char *version;
    char *date;
    char *url;
    char *author;
    //! interface version
    int ifver;
} module_info_t;

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

typedef struct cmds_modules_pools_s
{
    const char    *string;
    const char    *description;
    cmd_callback  function;
    uint32_t      hash;
    struct cmds_modules_pools_s *next;
} cmd_module_pool_t;

typedef struct module_s
{
    HMODULE handle;
    char	  name[256];
    cmd_module_pool_t *cmd_pool;
    module_info_t info;
    struct module_s *next;
} module_t;
