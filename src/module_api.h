#pragma once

#include "dynamic_strings.h"
#include "vkapi.h"
#include "cmd_handler.h"
#include "curl_wrap.h"
#include "db_api.h"

typedef enum
{
    MODULE_UNLOAD_ANYTIME, // not needed to call
    MODULE_UNLOAD_UNREGISTER_FUNC_CALL, // need call Module_Unload
    MODULE_NEVER // cant be unloaded at runtime
} module_load_type_t;

#define ENGINE_API_VERSION 0

#ifndef _VKBOT_MODULE
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

typedef struct module_s
{
  HMODULE handle;
  char	  name[256];
  module_info_t *info;
  struct module_s *next;
} module_t;
#endif

//typedef struct
//{
//    char *name;
//    char *version;
//    char *date;
//    char *url;
//    char *author;
//    int ifver; // interface version
//} module_info_t;

typedef struct
{

} module_cmds_t;

typedef struct
{
  void (*register_command) (module_info_t *info, const char *cmd_name, const char *description, cmd_function_callback callback);
  void (*unregister_command) (module_info_t *info, const char *cmd_name);

  string_t (*vkapi_call_method) (vkapi_handle *object, const char *method, string_t specific_args, vkapi_boolean result_need);
  void (*vkapi_send_message) (vkapi_handle *object, int peer_id, const char *msg, vkapi_attach *attaches, int attaches_len);
  vkapi_attach *(*vkapi_upload_doc_by_url) (vkapi_handle *object, vkapi_message_object *message, const char *filename, string_t data, docs_type_t type);

  string_t (*string_init) ( void );
  string_t (*string_dublicate) ( string_t s );
  void (*string_format) ( string_t s, const char *fmt, ...);
  void (*string_strncat) ( string_t s, const char *string, size_t size );
  void (*string_copy) ( string_t s, const char *string );
  void (*string_destroy) ( string_t s );

  void *(*curl_init) (void);
  vkapi_boolean (*curl_get) ( void *curl_handle, string_t url, string_t useragent, string_t dataptr );
  vkapi_boolean (*curl_post) (void *curl_handle, const char *url, string_t post, string_t useragent, string_t dataptr );
  vkapi_boolean (*curl_uploadfile) ( void *curl_handle, const char *url, const char *fieldname, const char *filename, string_t data, string_t useragent, string_t dataptr );
  void (*curl_cleanup) (void *ptr);

  void (*memcache_push) (const char *key, const char *value);
  const char* (*memcache_get) (const char *key);

  void (*db_register_type) ( const char *type, db_open_t open, db_exec_t exec, db_close_t close, void *data );
  db_handle_t *(*db_open) (const char *filename, const char *db_name, void *data);
  void (*db_exec) (db_handle_t *db, const char *cmd, db_callback_t callback, void *data);
  void (*db_close) (db_handle_t *db);

  int (*users_is_init) ();
  void (*users_module_register_users) (users_t *users);
  int (*users_get_privilage) (int id);
  const char *(*users_get_name_privilage)(int priv);

  unsigned int (*crc32_calc) (const unsigned char *buf, size_t len);
  void (*alert) (module_info_t *info, const char *fmt, ...);
} engine_api_t;
