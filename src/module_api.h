#pragma once

#include "curl_wrap.h"
#include "dynamic_strings.h"
#include "vkapi.h"

#include "module.h"
#include "module_cmds.h"
#include "module_events.h"

#define ENGINE_API_VERSION 3

//! Load module by name
bool module_load(const char* name);

//! Check module is loaded
bool module_loaded(const char* name);

//! Find module by name and get sym by GetProcAddress (dlsym)
void* module_function(const char* name, const char* function);

typedef struct
{
    void* (*malloc)(size_t size);
    void (*free)(void* ptr);

    void (*register_command)(const char* cmd_name, const char* description, cmd_callback callback);
    void (*register_event)(const char* event_name, event_handler_t handler);
    void (*register_event_hook)(const char* event_name, event_handler_t handler);

    string_t (*vkapi_call_method)(const char* method, curl_postfield_t args, bool result_need);
    void (*vkapi_send_message)(int peer_id, const char* msg, vkapi_attachment* attaches, int attaches_len);
    vkapi_attachment* (*vkapi_upload_attachment)(vkapi_message_object* message, const char* filename, string_t data, docs_type_t type);

    string_t (*string_init)(void);
    string_t (*string_dublicate)(string_t s);
    void (*string_format)(string_t s, const char* fmt, ...);
    void (*string_strncat)(string_t s, const char* string, size_t size);
    void (*string_copy)(string_t s, const char* string);
    void (*string_destroy)(string_t s);

    void* (*curl_init)(void);
    curl_postfield_t (*curl_postfield_init)(void);
    void (*curl_postfield_push)(curl_postfield_t pool, const char* key, const char* value);
    string_t (*curl_postfield_serialize)(curl_postfield_t pool);
    void (*curl_postfield_destroy)(curl_postfield_t pool);

    bool (*curl_get)(void* curl_handle, const char* url, string_t useragent, string_t dataptr);
    bool (*curl_post)(void* curl_handle, const char* url, string_t post, string_t useragent, string_t dataptr);
    bool (*curl_uploadfile)(void* curl_handle, const char* url, const char* fieldname, const char* filename, string_t data, string_t useragent, string_t dataptr);
    void (*curl_cleanup)(void* ptr);

    bool (*module_load)(const char* name);
    bool (*module_loaded)(const char* name);
    void* (*module_function)(const char* name, const char* function);

    unsigned int (*memcrc32)(const unsigned char* buf, size_t len);
    void (*alert)(const char* fmt, ...);
} engine_api_t;
