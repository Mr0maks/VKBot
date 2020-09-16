/*
vkapi_json_wrap.c - cjson utils for vkapi.c
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
