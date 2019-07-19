#include <memory.h>
#include "thpool.h"

#include "vkapi.h"
#include "vkapi_json_wrap.h"
#include "worker_queue.h"
#include "va_utils.h"
#include "cmd_handler.h"

#include <cJSON.h>
#include <unistd.h>

#define VK_GROUP_ID "180928573"
#define VK_GROUP_TOKEN "90128137076d3602a1d1dfda4da4cfb904ac15203a34e267c3457d26bd85e41790340221d65ab1a93cc7e"

#include <pthread.h>

static pthread_mutex_t mutex_lock;
static pthread_mutex_t mutex_lock_2;
static pthread_cond_t cond_var;

static threadpool worker_pool = NULL;

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

      vkapi_message_new_object *message = get_queue();

      pthread_mutex_unlock(&mutex_lock);

      if(message == NULL)
	goto start;

      printf("LONGPOLL!\n");

	 if(vkapi_object->group_id == (message->from_id * -1))
	    continue;

      printf("Message from %i ot %i : %s\n", message->peer_id, message->from_id, message->text->ptr);

      if(message->text->len < 256)
	{
	cmd_handle(vkapi_object, message);
	}
      else
	//vkapi_send_message(vkapi_object, message->peer_id, va("Ваше сообщение было проигнорировано из за большой длины (%lu). Допустимое количество символов : 256.", message->text->len));

      if(message->attachmens)
	cJSON_Delete(message->attachmens);

      destroy_string(message->text);

      free(message);
    }
}

int worker_get_workers_count()
{
  return thpool_num_threads_working(worker_pool);
}

void worker_main_thread( const char *token, const char *group_id, int num_workers )
{

  queue_init();
  cmd_handler_init();

  pthread_mutex_init(&mutex_lock, NULL);
  pthread_mutex_init(&mutex_lock_2, NULL);
  pthread_cond_init(&cond_var, NULL);

  worker_pool = thpool_init(num_workers);

  for(int i = 0; i < num_workers; i++ )
    {
      thpool_add_work( worker_pool, (void*)long_poll_worker, NULL );
    }

  vkapi_object *object = vk_api_init(VK_GROUP_TOKEN, VK_GROUP_ID);

  try_again:
    if(!vkapi_get_long_poll_server(object))
      {
	printf("Error while getting long poll server. I try again.\n");
	goto try_again;
      }

  while (1) {
      string_t long_poll_string = NULL;

  get_string:
      long_poll_string = vkapi_get_longpool_data(object);

      if( long_poll_string == NULL)
	goto get_string;

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
