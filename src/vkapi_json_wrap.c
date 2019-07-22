#include <cJSON.h>
#include "vkapi.h"

vkapi_bool json_vkapi_long_poll_have_updates( cJSON *main )
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

