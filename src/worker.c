#include <memory.h>

#include "thpool.h"

#include "vkapi.h"

#define VK_GROUP_ID "180928573"
#define VK_GROUP_TOKEN "90128137076d3602a1d1dfda4da4cfb904ac15203a34e267c3457d26bd85e41790340221d65ab1a93cc7e"

void worker_main_thread( const char *token, const char *group_id, int num_workers )
{

  //threadpool worker_pool = thpool_init(num_workers);

  vkapi_object *obj = vk_api_init(VK_GROUP_TOKEN, VK_GROUP_ID);

  while(1)
    {

      struct string *data = vkapi_get_longpool_data(obj);

      printf("LONGPOOL: %s\n", data->ptr);

      destroy_string(data);

    }
}
