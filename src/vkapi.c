#include <assert.h>
#include <string.h>
#include <cJSON.h>

#include "va_utils.h"
#include "vkapi.h"
#include "vk_strings.h"

#define VK_URL_METHOD "https://api.vk.com/method/"

size_t vk_string_writefunc(void *ptr, size_t size, size_t nmemb, void *data)
{
  string_t s = (string_t)data;

  if(ptr == NULL)
    return 0;

  string_strncat(s, (const char*)ptr, size*nmemb);

  return size*nmemb;
}

string_t vk_api_call_method(vkapi_object *object, const char *method, string_t specific_args, vkapi_bool result_need)
{
  string_t s = NULL;
  string_t s2 = NULL;

  s = string_init();
  s2 = string_init();

  if(specific_args)
  string_format( s2, "group_id=%i&access_token=%s&%s&v=5.101", object->group_id, object->vk_token, specific_args->ptr);
  else
  string_format( s2, "group_id=%i&access_token=%s&v=5.101", object->group_id, object->vk_token);

  curl_easy_setopt(object->curl_handle, CURLOPT_URL, va("%s/%s", VK_URL_METHOD, method));
  curl_easy_setopt(object->curl_handle, CURLOPT_POST, 1L);
  //curl_easy_setopt(object->curl_handle, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(object->curl_handle, CURLOPT_POSTFIELDS, s2->ptr );
  curl_easy_setopt(object->curl_handle, CURLOPT_POSTFIELDSIZE, s2->len );
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, vk_string_writefunc);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, s);

  CURLcode error_code = curl_easy_perform(object->curl_handle);

  string_destroy( s2 );

  if(error_code != CURLE_OK)
    {
      printf("vk_api: libcurl error: %s \n", curl_easy_strerror(error_code));
      string_destroy(s);
      return NULL;
    }

  if(!result_need)
    {
      string_destroy(s);
      return NULL;
    }

  return s;
}

string_t vkapi_get_longpoll_data(vkapi_object *object)
{
  string_t s = NULL;
  string_t s2 = NULL;

  s = string_init();
  s2 = string_init();

  string_format( s2, "act=a_check&key=%s&wait=25&mode=2&ts=%i", object->longpoll_key, object->longpoll_timestamp );

  curl_easy_setopt(object->curl_handle, CURLOPT_URL, object->longpoll_server_url );
  curl_easy_setopt(object->curl_handle, CURLOPT_POST, 1L);
  //curl_easy_setopt(object->curl_handle, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(object->curl_handle, CURLOPT_POSTFIELDS, s2->ptr );
  curl_easy_setopt(object->curl_handle, CURLOPT_POSTFIELDSIZE, s2->len );
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, vk_string_writefunc );
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, s );

  CURLcode error_code = curl_easy_perform(object->curl_handle);

  string_destroy( s2 );

  if(error_code != CURLE_OK)
    {
      printf("vk_api: libcurl error %s \n", curl_easy_strerror(error_code));
      string_destroy( s );
      return NULL;
    }

  cJSON *json = cJSON_ParseWithOpts(s->ptr, NULL, false );

  if( !json )
    {
      printf( "Error while getting long poll data: json parser return NULL\n");
      cJSON_Delete( json );
      string_destroy( s );
      return NULL;
    }

  cJSON *ts = cJSON_GetObjectItem(json, "ts");

  if(ts)
    object->longpoll_timestamp = atoi(cJSON_GetStringValue(ts));
  else
    {
      printf("Error while getting long poll data: json ts == NULL\n");
      cJSON_Delete(json);
      string_destroy( s );
      return NULL;
    }

  cJSON_Delete(json);

  return s;
}

void vkapi_send_message(vkapi_object *object, int peer_id, const char *msg)
{
  string_t s = string_init();
  string_format(s, "message=%s&random_id=0&peer_id=%i", msg, peer_id);
  vk_api_call_method(object, "messages.send", s, false);
  string_destroy(s);
}

vkapi_bool vkapi_get_long_poll_server(vkapi_object *object)
{
  string_t method_result = vk_api_call_method(object, "groups.getLongPollServer", NULL, true);

  if(!method_result)
    {
      printf("Error while getting long poll data: vk_api_call_method return NULL\n");
      return false;
    }

  const char *json_return;

  cJSON *json = cJSON_ParseWithOpts(method_result->ptr, &json_return, false );

  if(!json)
    {
      printf("Error while getting long poll data: json parser return NULL\n");
      printf("Error before: %s\n", json_return);
      cJSON_Delete(json);
      string_destroy(method_result);
      return false;
    }

  const cJSON *response = NULL;

  response = cJSON_GetObjectItem(json, "response");

  cJSON *key = cJSON_GetObjectItem(response, "key");
  cJSON *server = cJSON_GetObjectItem(response, "server");
  cJSON *timestamp = cJSON_GetObjectItem(response, "ts");

  if(key && server && timestamp)
    {
  strncpy(object->longpoll_key, cJSON_GetStringValue(key), sizeof(object->longpoll_key));
  strncpy(object->longpoll_server_url, cJSON_GetStringValue(server), sizeof(object->longpoll_server_url));
  object->longpoll_timestamp = atoi(cJSON_GetStringValue(timestamp));
    } else {
      printf("Error while getting long poll data: json parser return null json objects. Seems its error %s\n", method_result->ptr);
      cJSON_Delete(json);
      string_destroy(method_result);
      return false;
    }

  cJSON_Delete(json);
  string_destroy(method_result);
  return true;
}

vkapi_object *vk_api_init(const char *token, int group_id)
{
  vkapi_object *result = (vkapi_object*)calloc(1, sizeof(vkapi_object));

  if(!result)
    return NULL;

  result->curl_handle = curl_easy_init();

  strncpy(result->vk_token, token, sizeof(result->vk_token));
  result->group_id = group_id;

  printf("%s : %i\n", result->vk_token, result->group_id);

  return result;
}

void vk_api_destroy(vkapi_object *ptr)
{
  assert(ptr != NULL);

  curl_easy_cleanup(ptr->curl_handle);
  free(ptr);
}
