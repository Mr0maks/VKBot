#include "common.h"

bool vkapi_json_long_poll_have_updates( cJSON *main )
{
  cJSON *updates = cJSON_GetObjectItem(main, "updates");

  if( cJSON_IsArray(updates) )
    {
      if( cJSON_GetArrayItem(updates, 0) )
	return true;
      else
	return false;
    }

  return false;
}

int vkapi_json_parse_groups_getById( string_t s )
{
  int value = 0;
  cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false);

  if(!ptr)
    return value;

  cJSON *response = cJSON_GetObjectItem(ptr, "response");

  if(!cJSON_IsArray(response))
    {
      cJSON_Delete(ptr);
      return value;
    }

  cJSON *bot = cJSON_GetArrayItem(response, 0);

  if(!bot)
    {
      cJSON_Delete(ptr);
      return value;
    }

  cJSON *id = cJSON_GetObjectItem(bot, "id");

  value = id->valueint;

  cJSON_Delete(ptr);

  return value;
}
