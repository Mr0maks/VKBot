#include <assert.h>
#include <string.h>
#include <cJSON.h>

#include "va_utils.h"
#include "vkapi.h"
#include "dynamic_strings.h"

#include "curl_wrap.h"

#include <stdio.h>

#include "vkapi_json_wrap.h"

#define VK_URL_METHOD "https://api.vk.com/method/"

string_t vkapi_call_method(vkapi_handle *object, const char *method, string_t specific_args, vkapi_boolean result_need)
{
  string_t s = NULL;
  string_t s2 = NULL;

  vkapi_boolean error_code = false;

  s2 = string_init();

  if(specific_args)
  string_format( s2, "group_id=%i&access_token=%s&%s&v=5.101", object->group_id, object->vk_token, specific_args->ptr);
  else
  string_format( s2, "group_id=%i&access_token=%s&v=5.101", object->group_id, object->vk_token);

  if(result_need)
    {
      s = string_init();
      error_code = curl_post(object->curl_handle, va("%s/%s", VK_URL_METHOD, method), s2, NULL, s);
    } else {
      error_code = curl_post(object->curl_handle, va("%s/%s", VK_URL_METHOD, method), s2, NULL, NULL);
    }

  string_destroy( s2 );

  if(error_code != true)
    {
      printf("vk_api: libcurl error!\n");
      string_destroy(s);
      return NULL;
    }

  return s;
}

vkapi_attach *vkapi_upload_doc_by_url(vkapi_handle *object, vkapi_message_object *message, const char *filename, string_t data, docs_type_t type)
{
  string_t s = string_init();

  switch (type) {

      case VKAPI_DOC:
	{
	  string_format(s, "type=doc&peer_id=%i", message->peer_id);
	  string_t result = vkapi_call_method(object, "docs.getMessagesUploadServer", s, true );

	  if(!result)
	    break;

	  cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

	  if(!ptr)
	    break;

	  cJSON *upload_url = cJSON_GetObjectItem(cJSON_GetObjectItem(ptr, "response"), "upload_url");

	  if(!upload_url)
	    {
	    printf("upload url is null\n");
	    break;
	    }

	  string_t dataptr = string_init();

	  printf("UPLOAD TO URL %s\n", cJSON_GetStringValue(upload_url));

      curl_uploadfile(object->curl_handle, cJSON_GetStringValue(upload_url), "file", filename, data, NULL, dataptr);

	  cJSON_Delete(ptr);
	  string_destroy(result);

      ptr = cJSON_ParseWithOpts(dataptr->ptr, NULL, false);

      if(!ptr)
	    break;

      string_format(s, "file=%s", cJSON_GetStringValue(cJSON_GetObjectItem(ptr, "file")));

      cJSON_Delete(ptr);

	  result = vkapi_call_method(object, "docs.save", s, true );

      printf("HMMMM: %s\n", result->ptr);

      ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

      cJSON *resp = cJSON_GetObjectItem(ptr, "response");

      cJSON *doc = cJSON_GetObjectItem(resp, "doc");

      cJSON *owner_id = cJSON_GetObjectItem(doc, "owner_id");
      cJSON *media_id = cJSON_GetObjectItem(doc, "id");

      vkapi_attach *attach = calloc(1, sizeof(vkapi_attach));

      attach[0].type = VKAPI_DOC;
      attach[0].owner_id = owner_id->valueint;
      attach[0].media_id = media_id->valueint;

      cJSON_Delete(ptr);

	  string_destroy(s);
	  string_destroy(result);
      return attach;
	}
      case VKAPI_PHOTO:
    {	  string_format(s, "peer_id=%i", message->peer_id);
      string_t result = vkapi_call_method(object, "photos.getMessagesUploadServer", s, true );

      if(!result)
        break;

      cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

      if(!ptr)
        break;

      cJSON *upload_url = cJSON_GetObjectItem(cJSON_GetObjectItem(ptr, "response"), "upload_url");

      if(!upload_url)
        {
        printf("upload url is null\n");
        break;
        }

      string_t dataptr = string_init();

      printf("UPLOAD TO URL %s\n", cJSON_GetStringValue(upload_url));

      curl_uploadfile(object->curl_handle, cJSON_GetStringValue(upload_url), "photo", filename, data, NULL, dataptr);

      cJSON_Delete(ptr);
      string_destroy(result);

      ptr = cJSON_ParseWithOpts(dataptr->ptr, NULL, false);

      if(!ptr)
        break;

      string_format(s, "photo=%s&server=%i&hash=%s", cJSON_GetStringValue(cJSON_GetObjectItem(ptr, "photo")), cJSON_GetObjectItem(ptr, "server")->valueint, cJSON_GetStringValue(cJSON_GetObjectItem(ptr, "hash")));

      cJSON_Delete(ptr);

      result = vkapi_call_method(object, "photos.saveMessagesPhoto", s, true );

      printf("HMMMM: %s\n", result->ptr);

      ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

      cJSON *resp = cJSON_GetArrayItem(cJSON_GetObjectItem(ptr, "response"), 0);

      cJSON *owner_id = cJSON_GetObjectItem(resp, "owner_id");
      cJSON *media_id = cJSON_GetObjectItem(resp, "id");

      vkapi_attach *attach = calloc(1, sizeof(vkapi_attach));

      attach[0].type = VKAPI_PHOTO;
      attach[0].owner_id = owner_id->valueint;
      attach[0].media_id = media_id->valueint;

      cJSON_Delete(ptr);

      string_destroy(s);
      string_destroy(result);
      return attach;
	}

    }

  return NULL;
}

string_t vkapi_get_longpoll_data(vkapi_handle *object)
{
  string_t s = NULL;
  string_t s2 = NULL;

  s = string_init();
  s2 = string_init();

  string_format( s2, "act=a_check&key=%s&wait=25&mode=2&ts=%lli", object->longpoll_key, object->longpoll_timestamp );

  vkapi_boolean error_code = curl_post(object->curl_handle, object->longpoll_server_url, s2, NULL, s);

  string_destroy( s2 );

  if(error_code != true)
    {
      printf("vk_api: libcurl error!\n");
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
    object->longpoll_timestamp = atoll(cJSON_GetStringValue(ts));
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

const char *vkapi_attach_type(docs_type_t doc)
{
    switch (doc) {
    case VKAPI_PHOTO:
    {
        return "photo";
    }
    case VKAPI_VIDEO:
    {
        return "video";
    }
    case VKAPI_DOC:
    {
        return "doc";
    }
    case VKAPI_AUDIO:
    {
        return "audio";
    }
    case VKAPI_WALL:
    {
        return "wall";
    }

    case VKAPI_NULL:
    default:
    {
        return "unknown";
    }
    }
}

void vkapi_send_message(vkapi_handle *object, int peer_id, const char *msg, vkapi_attach *attachments, int attachmens_len)
{
  string_t s = string_init();

  if(!attachments)
      string_format(s, "message=%s&random_id=0&peer_id=%i", msg, peer_id);
  else {
      string_t formated_attachmens = string_init();
      string_t tmp = string_init();
      for(int i = 0; attachmens_len > i; i++)
      {
          if(i > 0)
              string_strncat(formated_attachmens, ",", 1);

          string_format(tmp, "%s%i_%i", vkapi_attach_type(attachments[i].type), attachments[i].owner_id, attachments[i].media_id);
          string_strncat(formated_attachmens, tmp->ptr, tmp->len);
      }

      if(!msg)
      string_format(s, "random_id=0&peer_id=%i&attachment=%s", peer_id, formated_attachmens->ptr);
      else
      string_format(s, "message=%s&random_id=0&peer_id=%i&attachment=%s", msg, peer_id, formated_attachmens->ptr);

      string_destroy(formated_attachmens);
      string_destroy(tmp);
  }

  vkapi_call_method(object, "messages.send", s, false);
  string_destroy(s);
}

vkapi_boolean vkapi_get_long_poll_server(vkapi_handle *object)
{
  string_t method_result = vkapi_call_method(object, "groups.getLongPollServer", NULL, true);

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
  object->longpoll_timestamp = atoll(cJSON_GetStringValue(timestamp));
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

static int vkapi_get_group_id(vkapi_handle *object)
{
  int value = 0;
  string_t s2 = string_init();
  string_t data = string_init();

  string_format(s2, "access_token=%s&v=5.101", object->vk_token);

  vkapi_boolean result = curl_post(object->curl_handle, VK_URL_METHOD"/groups.getById", s2, NULL, data);

  string_destroy(s2);

  if(result)
    {
      value = vkapi_json_parse_groups_getById(data);
    }

  string_destroy(data);

  return value;
}

vkapi_handle *vkapi_init(const char *token)
{
  vkapi_handle *result = (vkapi_handle*)calloc(1, sizeof(vkapi_handle));

  if(!result)
    return NULL;

  result->curl_handle = curl_init();

  strncpy(result->vk_token, token, sizeof(result->vk_token));
  result->group_id = vkapi_get_group_id(result);

  if(result->group_id == 0)
    {
      result = NULL;
      printf("vkapi error: failed to get group id!\n");
    }

  return result;
}

void vkapi_destroy(vkapi_handle *ptr)
{
  assert(ptr != NULL);

  curl_cleanup(ptr->curl_handle);
  free(ptr);
}
