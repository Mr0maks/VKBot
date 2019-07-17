#include <memory.h>
#include "thpool.h"

#include "vkapi.h"
#include "vkapi_json_wrap.h"
#include "worker_queue.h"
#include "va_utils.h"

#include <cJSON.h>
#include "vk_strings.h"
#include <unistd.h>

#define VK_GROUP_ID "180928573"
#define VK_GROUP_TOKEN "90128137076d3602a1d1dfda4da4cfb904ac15203a34e267c3457d26bd85e41790340221d65ab1a93cc7e"

#include <pthread.h>

pthread_mutex_t mutex_lock;
pthread_cond_t cond_var;

void long_poll_worker(void *data)
{

  vkapi_object *vkapi_object = vk_api_init(VK_GROUP_TOKEN, VK_GROUP_ID);

  while (1) {
      start:

      pthread_mutex_lock(&mutex_lock);

      while(queue_is_empty())
	{
	printf("nah\n");
	pthread_cond_wait(&cond_var, &mutex_lock);
	}

      cJSON *main_obj = get_queue();

      pthread_mutex_unlock(&mutex_lock);

      if(main_obj == NULL)
	goto start;

      printf("LONGPOLL!\n");

      cJSON *object = cJSON_GetObjectItem(main_obj, "object");
      cJSON *text = cJSON_GetObjectItem(object, "text");
      cJSON *peer_id = cJSON_GetObjectItem(object, "peer_id");
      cJSON *id = cJSON_GetObjectItem(object, "from_id");

	 if(atoi(vkapi_object->group_id) == (id->valueint * -1))
	    continue;

      printf("Message from %i ot %i : %s\n", peer_id->valueint, id->valueint, cJSON_GetStringValue(text));

      vkapi_send_message(vkapi_object, peer_id->valueint, va("Ты написал %s", cJSON_GetStringValue(text)), "Remove This");

      cJSON_Delete(main_obj);
    }
}

void worker_main_thread( const char *token, const char *group_id, int num_workers )
{

  queue_init();

  pthread_mutex_init(&mutex_lock, NULL);
  pthread_cond_init(&cond_var, NULL);

  threadpool worker_pool = thpool_init(num_workers);

  for(int i = 0; i < num_workers; i++ )
    {
      thpool_add_work( worker_pool, (void*)long_poll_worker, NULL );
    }

  vkapi_object *object = vk_api_init(VK_GROUP_TOKEN, VK_GROUP_ID);

  while (1) {

      struct string *long_poll_string = vkapi_get_longpool_data(object);

      cJSON *main_obj = cJSON_ParseWithOpts(long_poll_string->ptr, NULL, false);

      printf("%s\n", long_poll_string->ptr);

      if(!json_vkapi_long_poll_have_updates(main_obj))
	{
	  cJSON_Delete(main_obj);
	  destroy_string(long_poll_string);
	  continue;
	}

      cJSON *updates = cJSON_GetObjectItem(main_obj, "updates");
      cJSON *update_block = NULL;

      pthread_mutex_lock(&mutex_lock);

      cJSON_ArrayForEach(update_block, updates)
      {

	cJSON *copy = cJSON_Duplicate(update_block, true);
	if(!copy)
	  break;

	add_queue(copy);

      }

      pthread_mutex_unlock(&mutex_lock);
      pthread_cond_broadcast(&cond_var);

      cJSON_Delete(main_obj);
    }

  thpool_wait(worker_pool);

}
