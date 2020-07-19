#ifndef ENGINECALLBACKS_H
#define ENGINECALLBACKS_H

#include <module_api.h>

extern engine_api_t engine_api;

#define REGISTER_COMMAND (*engine_api.register_command)
#define UNREGISTER_COMMAND (*engine_api.unregister_command)

#define VKAPI_CALL_METHOD (*engine_api.vkapi_call_method)
#define VKAPI_SEND_MESSAGE (*engine_api.vkapi_send_message)
#define VKAPI_UPLOAD_DOC (*engine_api.vkapi_upload_doc_by_url)

#define VKAPI_SEND_MESSAGE_ERROR(x) VKAPI_SEND_MESSAGE(message->peer_id, va("VKBot\nСообщите о ошибке разработчику\n%s:%i: %s: Ошибка `%s'", __FILE__, __LINE__, __func__, x), NULL, 0)
#define VKAPI_ASSERT(e) ((e) ? (void)0 : VKAPI_SEND_MESSAGE(message->peer_id, va("VKBot\nСообщите о ошибке разработчику\n%s:%i: %s: Ошибка `Assertation `%s` failed'", __FILE__, __LINE__, __func__, #e), NULL, 0))

#define STRING_INIT (*engine_api.string_init)
#define STRING_DUBLICATE (*engine_api.string_dublicate)
#define STRING_FORMAT (*engine_api.string_format)
#define STRING_STRNCAT (*engine_api.string_strncat)
#define STRING_COPY (*engine_api.string_copy)
#define STRING_DESTROY (*engine_api.string_destroy)

#define CURL_POSTFIELD_INIT (*engine_api.curl_postfield_init)
#define CURL_POSTFIELD_PUSH (*engine_api.curl_postfield_push)
#define CURL_POSTFIELD_SERIALIZE (*engine_api.curl_postfield_push)
#define CURL_POSTFIELD_DESTROY (*engine_api.curl_postfield_destroy)

#define CURL_INIT (*engine_api.curl_init)
#define CURL_GET (*engine_api.curl_get)
#define CURL_POST (*engine_api.curl_post)
#define CURL_UPLOAD_FILE (*engine_api.curl_uploadfile)
#define CURL_CLEANUP (*engine_api.cleanup)

#define REGISTER_EVENT (*engine_api.register_event)
#define REGISTER_EVENT_HOOK (*engine_api.register_event_hook)

#define MODULE_LOAD (*engine_api.module_load)
#define MODULE_LOADED (*engine_api.module_loaded)
#define MODULE_FUNCTION (*engine_api.module_function)

#define CRC32 (*engine_api.memcrc32)
#define ALERT (*engine_api.alert)

//#define malloc (*engine_api.malloc)
//#define calloc(x,c) (*engine_api.malloc)(x,c)
//#define free (*engine_api.free)




#endif // ENGINECALLBACKS_H
