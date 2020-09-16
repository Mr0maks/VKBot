#include <enginecallbacks.h>
#include <cJSON.h>
#include "va_utils.h"

void cmd_cat(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = STRING_INIT();

  if(CURL_GET(NULL, "https://api.thecatapi.com/v1/images/search", NULL, s) != true)
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      STRING_DESTROY(s);
      return;
    }

  cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false);

  if(!ptr)
    {
      VKAPI_SEND_MESSAGE_ERROR("Сервер api.thecatapi.com недоступен");
      STRING_DESTROY(s);
      return;
    }

  cJSON *object = cJSON_GetArrayItem(ptr, 0);

  cJSON *url = cJSON_GetObjectItem(object, "url");

  string_t filedata = STRING_INIT();

  CURL_GET(NULL, cJSON_GetStringValue(url), NULL, filedata);

  vkapi_attachment *attach = VKAPI_UPLOAD_ATTACHMENT( message, "random_cat.png", filedata, VKAPI_PHOTO);

  VKAPI_SEND_MESSAGE( message->peer_id, NULL, attach, 1);

  free(attach);

  cJSON_Delete(ptr);

  STRING_DESTROY(s);
  STRING_DESTROY(filedata);
}
