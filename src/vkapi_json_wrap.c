#include <cJSON.h>

#include "vkapi.h"

vkapi_bool json_vkapi_is_error( cJSON *main )
{

}

typedef struct
{
  const char *string;
  vkapi_long_pool_type type;
} json_vkapi_long_poll_type;

vkapi_long_pool_type json_vkapi_long_poll_msg_type( cJSON *main )
{

}

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

