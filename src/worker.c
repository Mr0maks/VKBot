#include <memory.h>
#include <time.h>
#include <sys/time.h>

#include "thpool.h"

#include "vkapi.h"
#include "vkapi_json_wrap.h"
#include "vk_setting.h"
#include "worker_queue.h"
#include "va_utils.h"
#include "cmd_handler.h"

#include <cJSON.h>
#include <unistd.h>

#include <pthread.h>

#include "memcache.h"

static pthread_mutex_t mutex_lock;
static pthread_mutex_t command_handler_mutex;
static pthread_cond_t cond_var;

static threadpool worker_pool = NULL;

static volatile size_t command_processed;
static volatile size_t message_processed;

static volatile vkapi_bool main_thread_loop = false;

typedef struct
{
  int worker_id;
  vkapi_bool loop;
  vkapi_handle *vkapi_object;
} worker_data_t;

void long_poll_worker( void *data )
{
  worker_data_t *worker_data = (worker_data_t*)data;

  printf("[Worker %i] OK!\n", worker_data->worker_id);

  while (worker_data->loop) {
      start:

	sleep( 1 );

      pthread_mutex_lock( &mutex_lock );

      while( queue_empty() )
	{
	pthread_cond_wait( &cond_var, &mutex_lock );
	}

      vkapi_message_object *message = queue_pop();

      pthread_mutex_unlock( &mutex_lock );

      if( message == NULL )
	goto start;

	 if( worker_data->vkapi_object->group_id == (message->from_id * -1) )
	   {
	     if( message->attachmens )
	       cJSON_Delete( message->attachmens );

	     string_destroy( message->text );

	     free( message );
	    continue;
	   }

      pthread_mutex_lock(&command_handler_mutex);
      struct timeval tv1;
      gettimeofday( &tv1, NULL );
      long long int ns1 = ((tv1.tv_sec*1000LL) + tv1.tv_usec);

      if( message->text->len < 512 || message->from_id < 0 )
	{
	  printf( "[Worker %i] Message peer_id: %i from_id: %i message: %s\n", worker_data->worker_id, message->peer_id, message->from_id, message->text->ptr );

	  if(cmd_handle( worker_data->vkapi_object, message ))
	    {
	      struct timeval tv2;
	      gettimeofday( &tv2, NULL );
	      long long int ns2 = ((tv2.tv_sec*1000LL) + tv2.tv_usec);
	      double time = (double)((ns2 - ns1) / 1000);
	      command_processed++;
	      printf("[Worker %i] message done in %f sec\n", worker_data->worker_id, time / 1000 );
	    }
	}

      message_processed++;

      pthread_mutex_unlock(&command_handler_mutex);

      if( message->attachmens )
	cJSON_Delete( message->attachmens );

      string_destroy( message->text );

      free( message );
    }
}

int worker_get_workers_count()
{
  return thpool_num_threads_working(worker_pool);
}

size_t worker_commands_processed()
{
  return command_processed;
}

size_t worker_message_processed()
{
  return message_processed;
}

void worker_exit()
{
  main_thread_loop = 0;
}

void load_modules();

void worker_main_thread( const char *token, int group_id, int num_workers )
{
  queue_init();
  load_modules();
  cmd_handler_init();
  memcache_init(64);

  command_processed = 0;
  message_processed = 0;

  main_thread_loop = true;

  pthread_mutex_init( &mutex_lock, NULL );
  pthread_mutex_init( &command_handler_mutex, NULL );
  pthread_cond_init( &cond_var, NULL );

  worker_data_t *work_data = calloc(sizeof(worker_data_t), num_workers + 1);

  worker_pool = thpool_init( num_workers );

  for( int i = 0; i < num_workers + 1; i++ )
    {
      work_data[i].worker_id = i;
      work_data[i].loop = true;
      work_data[i].vkapi_object = vkapi_init( VK_GROUP_TOKEN, VK_GROUP_ID );
    }

  for( int i = 0; i < num_workers; i++ )
    {
      thpool_add_work( worker_pool, long_poll_worker, &work_data[i] );
    }

  vkapi_handle *object = vkapi_init(VK_GROUP_TOKEN, VK_GROUP_ID);

try_again:
  if(!vkapi_get_long_poll_server(object))
    {
      printf("Error while getting long poll server. I try again.\n");
      sleep(10);
      goto try_again;
    }

  while (1) {
      string_t long_poll_string = NULL;

      long_poll_string = vkapi_get_longpoll_data( object );

      if( long_poll_string == NULL )
	goto try_again;

      cJSON *main_obj = cJSON_ParseWithOpts( long_poll_string->ptr, NULL, false );

      printf( "%s\n", long_poll_string->ptr );

      if( !vkapi_json_long_poll_have_updates( main_obj ) )
	{
	  cJSON_Delete( main_obj );
	  string_destroy( long_poll_string );
	  continue;
	}

      cJSON *updates = cJSON_GetObjectItem( main_obj, "updates" );
      cJSON *update_block = NULL;

      pthread_mutex_lock( &mutex_lock );

      cJSON_ArrayForEach( update_block, updates )
      {
	cJSON *copy = cJSON_Duplicate( update_block, true );

	if( !copy )
	  break;

	queue_push( copy );
      }

      pthread_mutex_unlock( &mutex_lock );
      pthread_cond_broadcast( &cond_var );

      cJSON_Delete( main_obj );
      string_destroy( long_poll_string );
    }

  vkapi_destroy(object);

  thpool_wait( worker_pool );
  thpool_pause( worker_pool );
  thpool_destroy( worker_pool );
  queue_deinit();
  cmd_handler_deinit();

  for( int i = 0; i < num_workers; i++ )
    {
      vkapi_destroy(work_data[i].vkapi_object);
    }

  free(work_data);
}
