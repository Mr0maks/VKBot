#include <assert.h>
#include <string.h>
#include <cJSON.h>

#include "va_utils.h"
#include "vkapi.h"
#include "strings.h"

#define VK_URL_METHOD "https://api.vk.com/method/"

size_t writefunc(void *ptr, size_t size, size_t nmemb, void *s)
{

  if(ptr == NULL)
    return 0;

  strncat_to_string( (struct string *)s, ptr, size*nmemb );

  return size*nmemb;
}

struct string *vk_api_call_method(vkapi_object *object, const char *method, const char *args)
{
  struct string *s = NULL; //, *s2 = NULL;

  s = init_string();
  //s2 = init_string();

  if(!s) // || !s2)
    return NULL;

  curl_easy_setopt(object->curl_handle, CURLOPT_URL, va("%s/%s", VK_URL_METHOD, method));
  curl_easy_setopt(object->curl_handle, CURLOPT_POST, 1L);
  //curl_easy_setopt(object->curl_handle, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(object->curl_handle, CURLOPT_POSTFIELDS, args);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, s);
  //curl_easy_setopt(object->curl_handle, CURLOPT_HEADERDATA, s2);

  CURLcode error_code = curl_easy_perform(object->curl_handle);

  printf("\n%s\n", s->ptr);
  //printf("\n%s\n", s2->ptr);

  if(error_code != CURLE_OK)
    {
      printf("vk_api: libcurl error %s \n", curl_easy_strerror(error_code));
      destroy_string(s);
      //destroy_string(s2);
      return NULL;
    }

  //destroy_string(s2);

  return s;
}

struct string *vkapi_get_longpool_data(vkapi_object *object)
{
  struct string *s = NULL;

  s = init_string();

  if( !s )
    return NULL;

  curl_easy_setopt(object->curl_handle, CURLOPT_URL, object->longpool_server_url);
  curl_easy_setopt(object->curl_handle, CURLOPT_POST, 1L);
  //curl_easy_setopt(object->curl_handle, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(object->curl_handle, CURLOPT_POSTFIELDS, va( "act=a_check&key=%s&wait=25&mode=2&ts=%i", object->longpool_key, object->longpool_timestamp ) );
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, s);

  CURLcode error_code = curl_easy_perform(object->curl_handle);

  printf("\n%s\n", s->ptr);

  if(error_code != CURLE_OK)
    {
      printf("vk_api: libcurl error %s \n", curl_easy_strerror(error_code));
      destroy_string( s );
      return NULL;
    }

  cJSON *json = cJSON_ParseWithOpts(s->ptr, NULL, false );

  if(!json)
    {
      printf("Error while getting long pool data: json parser return NULL\n");
      cJSON_Delete(json);
      return NULL;
    }

  cJSON *ts = cJSON_GetObjectItem(json, "ts");

  object->longpool_timestamp = atoi(cJSON_GetStringValue(ts));

  cJSON_Delete(json);

  return s;
}

bool vkapi_is_error(cJSON *json)
{

}

vkapi_object *vk_api_init(const char *token, const char *group_id)
{
  vkapi_object *result = (vkapi_object*)calloc(1, sizeof(vkapi_object));

  if(!result)
    return NULL;

  result->curl_handle = curl_easy_init();

  strncpy(result->vk_token, token, sizeof(result->vk_token));
  strncpy(result->group_id, group_id, sizeof(result->group_id));

  struct string *method_result = vk_api_call_method(result, "groups.getLongPollServer", va("group_id=%s&access_token=%s&v=5.101", result->group_id, result->vk_token ));

  if(!method_result)
    {
      printf("Error while initing vk api: vk_api_call_method return NULL\n");
      curl_easy_cleanup(result->curl_handle);
      free(result);
	return NULL;
    }

  const char *json_return;

  cJSON *json = cJSON_ParseWithOpts(method_result->ptr, &json_return, false );

  if(!json)
    {
      printf("Error while initing vk api: json parser return NULL\n");
      printf("Error before: %s\n", json_return);
      cJSON_Delete(json);
      curl_easy_cleanup(result->curl_handle);
      free(result);
      return NULL;
    }

  const cJSON *response = NULL;

  response = cJSON_GetObjectItem(json, "response");

  cJSON *key = cJSON_GetObjectItem(response, "key");
  cJSON *server = cJSON_GetObjectItem(response, "server");
  cJSON *timestamp = cJSON_GetObjectItem(response, "ts");

  strncpy(result->longpool_key, cJSON_GetStringValue(key), sizeof(result->longpool_key));
  strncpy(result->longpool_server_url, cJSON_GetStringValue(server), sizeof(result->longpool_server_url));
  result->longpool_timestamp = atoi(cJSON_GetStringValue(timestamp));

  printf("key is %s\nserver is %s\nts is %i\n", result->longpool_key, result->longpool_server_url, result->longpool_timestamp);

  cJSON_Delete(json);

  return result;
}

void vk_api_destroy(vkapi_object *ptr)
{
  assert(ptr == NULL);

  free(ptr);
}
