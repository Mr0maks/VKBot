/*
vkapi.c - VK API
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "common.h"

static string_t _vkapi_call_method(vkapi_handle *object, const char *method, curl_postfield_t args, bool result_need)
{
  string_t s = NULL;
  string_t s2 = NULL;
  bool args_null = (args == NULL);
  bool error_code = false;

  if(!object)
      object = worker_get_vkapi_handle();

  if (args_null)
      args = curl_postfield_init();

  curl_postfield_push(args, "v", VK_API_VERSION);
  curl_postfield_push(args, "access_token", object->vk_token);
  curl_postfield_push(args, "group_id", va("%i", object->group_id));

  s2 = curl_postfield_serialize(args);

  if(args_null)
      curl_postfield_destroy(args);

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
      Con_Printf("vk_api: libcurl error!\n");
      string_destroy(s);
      return NULL;
    }

  return s;
}

string_t vkapi_call_method(const char *method, curl_postfield_t args, bool result_need)
{
  vkapi_handle *object = worker_get_vkapi_handle();

  return _vkapi_call_method(object, method, args, result_need);
}

vkapi_attachment *vkapi_upload_attachment(vkapi_message_object *message, const char *filename, string_t data, docs_type_t type)
{
  vkapi_handle *object = worker_get_vkapi_handle();
  string_t result = NULL;

  curl_postfield_t args = curl_postfield_init();
  curl_postfield_push(args, "peer_id", va("%i", message->peer_id));

  switch (type) {
  case VKAPI_DOC:
  {
      curl_postfield_push(args, "type", "doc");
      result = vkapi_call_method("docs.getMessagesUploadServer", args, true );
      break;
  }
  case VKAPI_PHOTO:
  {
      result = vkapi_call_method("photos.getMessagesUploadServer", args, true );
      break;
  }
  case VKAPI_AUDIO:
  case VKAPI_VIDEO:
  case VKAPI_WALL:
  case VKAPI_NULL:
  default:
  {
      assert(0);
  }
  }

  curl_postfield_destroy(args);

  if(!result)
      return NULL;

	  cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

      if(!ptr)
      {
          return NULL;
      }

	  cJSON *upload_url = cJSON_GetObjectItem(cJSON_GetObjectItem(ptr, "response"), "upload_url");

      if(!upload_url)
      {
          return NULL;
      }

	  string_t dataptr = string_init();

      switch (type) {
      case VKAPI_DOC:
      {
          curl_uploadfile(object->curl_handle, cJSON_GetStringValue(upload_url), "file", filename, data, NULL, dataptr);
          break;
      }
      case VKAPI_PHOTO:
      {
          curl_uploadfile(object->curl_handle, cJSON_GetStringValue(upload_url), "file", filename, data, NULL, dataptr);
          break;
      }
      case VKAPI_AUDIO:
      case VKAPI_VIDEO:
      case VKAPI_WALL:
      case VKAPI_NULL:
      default:
      {
          assert(0);
      }
      }

	  cJSON_Delete(ptr);
	  string_destroy(result);

      ptr = cJSON_ParseWithOpts(dataptr->ptr, NULL, false);

      if(!ptr)
      {
          string_destroy(dataptr);
          return NULL;
      }

      args = curl_postfield_init();

      switch (type) {
      case VKAPI_DOC:
      {
      curl_postfield_push(args, "file", cJSON_GetStringValue(cJSON_GetObjectItem(ptr, "file")));
      result = vkapi_call_method("docs.save", args, true );
      break;
      }
      case VKAPI_PHOTO:
      {
      curl_postfield_push(args, "photo", cJSON_GetStringValue(cJSON_GetObjectItem(ptr, "photo")));
      curl_postfield_push(args, "server", va("%i", cJSON_GetObjectItem(ptr, "server")->valueint));
      curl_postfield_push(args, "hash", cJSON_GetStringValue(cJSON_GetObjectItem(ptr, "hash")));

      result = vkapi_call_method("photos.saveMessagesPhoto", args, true );
      break;
      }
      case VKAPI_AUDIO:
      case VKAPI_VIDEO:
      case VKAPI_WALL:
      case VKAPI_NULL:
      default:
      {
          assert(0);
      }
      }

      cJSON_Delete(ptr);
      string_destroy(dataptr);
      curl_postfield_destroy(args);

      ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

      cJSON *resp = cJSON_GetObjectItem(ptr, "response");
      cJSON *media_id = NULL;
      cJSON *owner_id = NULL;

      switch (type) {
      case VKAPI_DOC:
      {
          cJSON *doc = cJSON_GetObjectItem(resp, "doc");
          owner_id = cJSON_GetObjectItem(doc, "owner_id");
          media_id = cJSON_GetObjectItem(doc, "id");
          break;
      }
      case VKAPI_PHOTO:
      {
          cJSON *photo = cJSON_GetArrayItem(resp, 0);
          owner_id = cJSON_GetObjectItem(photo, "owner_id");
          media_id = cJSON_GetObjectItem(photo, "id");
          break;
      }
      case VKAPI_AUDIO:
      case VKAPI_VIDEO:
      case VKAPI_WALL:
      case VKAPI_NULL:
      default:
      {
          assert(0);
      }
      }

      vkapi_attachment *attachment = malloc(sizeof(vkapi_attachment));

      attachment[0].type = type;
      attachment[0].owner_id = owner_id->valueint;
      attachment[0].media_id = media_id->valueint;

      cJSON_Delete(ptr);
	  string_destroy(result);
      return attachment;
}

string_t vkapi_get_longpoll_data(vkapi_handle *object)
{
  string_t s = string_init();
  string_t s2 = NULL;

  curl_postfield_t args = curl_postfield_init();
  curl_postfield_push(args, "act", "a_check");
  curl_postfield_push(args, "key", object->longpoll_key);
  curl_postfield_push(args, "wait", "25");
  curl_postfield_push(args, "mode", "2");
  curl_postfield_push(args, "ts", va("%lli", object->longpoll_timestamp));

  s2 = curl_postfield_serialize(args);

  bool error_code = curl_post(object->curl_handle, object->longpoll_server_url, s2, NULL, s);

  curl_postfield_destroy(args);
  string_destroy( s2 );

  if(error_code != true)
    {
      Con_Printf("vk_api: libcurl error!\n");
      string_destroy( s );
      return NULL;
    }

  return s;
}

static inline const char *vkapi_attach_type(docs_type_t doc)
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

void vkapi_send_message(int peer_id, const char *msg, vkapi_attachment *attachments, int attachmens_len)
{
  curl_postfield_t args = curl_postfield_init();
  string_t formated_attachmens = string_init();
  const char *message_encoded = NULL;

  if (attachments)
  {
      string_t tmp = string_init();
      for(int i = 0; attachmens_len > i; i++)
      {
          if(i > 0)
              string_strncat(formated_attachmens, ",", 1);

          string_format(tmp, "%s%i_%i", vkapi_attach_type(attachments[i].type), attachments[i].owner_id, attachments[i].media_id);
          string_strncat(formated_attachmens, tmp->ptr, tmp->len);
      }
      string_destroy(tmp);
  }

      if(msg)
      {
          message_encoded = curl_urlencode(msg);
          curl_postfield_push(args, "message", message_encoded);
      }

      curl_postfield_push(args, "random_id", "0");
      curl_postfield_push(args, "peer_id", va("%i", peer_id));

      if(attachments)
      {
          curl_postfield_push(args, "attachment", formated_attachmens->ptr);
      }

  vkapi_call_method("messages.send", args, false);
  string_destroy(formated_attachmens);
  curl_postfield_destroy(args);
  if(message_encoded)
      free(message_encoded);
}

bool vkapi_get_long_poll_server(vkapi_handle *object)
{
  string_t method_result = _vkapi_call_method(object, "groups.getLongPollServer", NULL, true);

  if(!method_result)
    {
      Con_Printf("Error while getting long poll data: vk_api_call_method return NULL\n");
      return false;
    }

  cJSON *json = cJSON_ParseWithOpts(method_result->ptr, NULL, false );

  if(!json)
    {
      Con_Printf("Error while getting long poll data: json parser return NULL\n");
      cJSON_Delete(json);
      string_destroy(method_result);
      return false;
    }

  const cJSON *response = NULL;

  response = cJSON_GetObjectItem(json, "response");

  cJSON *key = cJSON_GetObjectItem(response, "key");
  cJSON *server = cJSON_GetObjectItem(response, "server");
  cJSON *timestamp = cJSON_GetObjectItem(response, "ts");

  if(key != NULL && server != NULL && timestamp != NULL)
    {
  strncpy(object->longpoll_key, cJSON_GetStringValue(key), sizeof(object->longpoll_key));
  strncpy(object->longpoll_server_url, cJSON_GetStringValue(server), sizeof(object->longpoll_server_url));
  object->longpoll_timestamp = atoll(cJSON_GetStringValue(timestamp));
    } else {
      Con_Printf("Error while getting long poll data: json parser return null json objects. Seems its error %s\n", method_result->ptr);
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

  string_t result = _vkapi_call_method(object, "groups.getById", NULL, true );

  if(result)
    {
      value = vkapi_json_parse_groups_getById(result);
    }

  string_destroy(result);

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
      Con_Printf("vkapi error: failed to get group id!\n");
    }

  return result;
}

void vkapi_destroy(vkapi_handle *ptr)
{
  assert(ptr);

  curl_cleanup(ptr->curl_handle);
  free(ptr);
}
