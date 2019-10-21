#include <enginecallbacks.h>
#include <cJSON.h>

void cmd_hlmemes(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  VKAPI_SEND_MESSAGE( message->peer_id, "Держи: https://www.youtube.com/playlist?list=PLxdH4XPU7OZnR5XV-b1QmnM7yCwAlmOUd", NULL, 0);
}

void cmd_cat(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = STRING_INIT();
  string_t url = STRING_INIT();

  STRING_COPY(url, "https://aws.random.cat/meow");

  if(CURL_GET(NULL, url, NULL, s) != true)
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      STRING_DESTROY(s);
      STRING_DESTROY(url);
      return;
    }

  cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false);

  if(!ptr)
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      STRING_DESTROY(s);
      STRING_DESTROY(url);
      return;
    }

  cJSON *file = cJSON_GetObjectItem(ptr, "file");

  STRING_COPY(url, cJSON_GetStringValue(file));

  string_t filedata = STRING_INIT();

  CURL_GET(NULL, url, NULL, filedata);

  vkapi_attach *attach = VKAPI_UPLOAD_DOC( message, "random_cat.png", filedata, VKAPI_PHOTO);

  VKAPI_SEND_MESSAGE( message->peer_id, NULL, attach, 1);

  cJSON_Delete(ptr);

  STRING_DESTROY(s);
  STRING_DESTROY(filedata);
  STRING_DESTROY(url);
}
