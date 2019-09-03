#ifndef ENGINECALLBACKS_H
#define ENGINECALLBACKS_H

#include <module_api.h>

extern engine_api_t engine_api;

#define REGISTER_COMMAND (*engine_api.register_command)
#define UNREGISTER_COMMAND (*engine_api.unregister_command)

#define VKAPI_CALL_METHOD (*engine_api.vkapi_call_method)
#define VKAPI_SEND_MESSAGE (*engine_api.vkapi_send_message)
#define VKAPI_UPLOAD_DOC (*engine_api.vkapi_upload_doc_by_url)

#define STRING_INIT (*engine_api.string_init)
#define STRING_DUBLICATE (*engine_api.string_dublicate)
#define STRING_FORMAT (*engine_api.string_format)
#define STRING_STRNCAT (*engine_api.string_strncat)
#define STRING_COPY (*engine_api.string_copy)
#define STRING_DESTROY (*engine_api.string_destroy)

#define CURL_INIT (*engine_api.curl_init)
#define CURL_GET (*engine_api.curl_get)
#define CURL_POST (*engine_api.curl_post)
#define CURL_UPLOAD_FILE (*engine_api.curl_uploadfile)
#define CURL_CLEANUP (*engine_api.cleanup)

#define MEMCACHE_PUSH (*engine_api.memcache_push)
#define MEMCACHE_GET (*engine_api.memcache_get)

#define CRC32 (*engine_api.crc32_calc)
#define ALERT (*engine_api.alert)

#endif // ENGINECALLBACKS_H
