#include <memory.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>

#include "thpool.h"

#include "vkapi.h"
#include "vkapi_json_wrap.h"
#include "vk_setting.h"
#include "worker_queue.h"
#include "va_utils.h"
#include "cmd_handler.h"
#include "users.h"
#include "sqlite3_db.h"
#include <cJSON.h>
#include <unistd.h>

#include <pthread.h>

#include "memcache.h"

#include "gc_memmgr.h"

static pthread_mutex_t mutex_lock;
static pthread_mutex_t command_handler_mutex;
static pthread_cond_t cond_var;

static threadpool worker_pool = NULL;

static volatile size_t command_processed;
static volatile size_t message_processed;

static volatile vkapi_boolean main_thread_loop = false;

typedef struct
{
  int worker_id;
  vkapi_boolean loop;
  vkapi_handle *vkapi_object;
} worker_data_t;

double get_time_s( void );

void long_poll_worker( void *data )
{
  worker_data_t *worker_data = (worker_data_t*)data;

  printf("[Worker %i] OK!\n", worker_data->worker_id);

  while (worker_data->loop) {

	sleep( 1 );

      vkapi_message_object *message = NULL;

      pthread_mutex_lock( &mutex_lock );

      while( message == NULL )
	{
	pthread_cond_wait( &cond_var, &mutex_lock );
	message = queue_pop();
	}

      pthread_mutex_unlock( &mutex_lock );

      assert(message);

	 if( worker_data->vkapi_object->group_id == (message->from_id * -1) )
	   {
	     if( message->attachmens )
	       cJSON_Delete( message->attachmens );

	     string_destroy( message->text );

	     free( message );
         CHECK_LEAKS();
	    continue;
	   }

      //pthread_mutex_lock(&command_handler_mutex);
	 double start_time = get_time_s();

      if( message->text->len < 512)
	{
	  printf( "[Worker %i] Message peer_id: %i from_id: %i message: %s\n", worker_data->worker_id, message->peer_id, message->from_id, message->text->ptr );

	  if(cmd_handle( worker_data->vkapi_object, message ))
	    {
	      //command_processed++;
	      printf("[Worker %i] took at %f sec\n", worker_data->worker_id, get_time_s() - start_time );
	    }
	}

      //message_processed++;

      //pthread_mutex_unlock(&command_handler_mutex);

      if( message->attachmens )
	cJSON_Delete( message->attachmens );

      string_destroy( message->text );

      free( message );
      CHECK_LEAKS();
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
//  GC_set_find_leak(1);

  load_modules();

  GC_INIT()

  GC_enable_incremental();

  queue_init();
  cmd_handler_init();
  memcache_init(64);

  command_processed = 0;
  message_processed = 0;

  main_thread_loop = true;

  pthread_mutex_init( &mutex_lock, NULL );
  pthread_mutex_init( &command_handler_mutex, NULL );
  pthread_cond_init( &cond_var, NULL );

  db_handle_t *user_db = db_sqlite_open("./users.sqlite3");

  db_sqlite_exec(user_db, DB_CREATE_TABLE_MAIN, NULL, NULL);

  printf("BD INIT OK ?\n");

  users_set_db(user_db);

  worker_data_t *work_data = calloc(sizeof(worker_data_t), num_workers + 1);

  worker_pool = thpool_init( num_workers );

  for( int i = 0; i < num_workers + 1; i++ )
    {
      work_data[i].worker_id = i;
      work_data[i].loop = true;
      work_data[i].vkapi_object = vkapi_init( VK_GROUP_TOKEN );
    }

  for( int i = 0; i < num_workers; i++ )
    {
      thpool_add_work( worker_pool, long_poll_worker, &work_data[i] );
    }

  vkapi_handle *object = vkapi_init(VK_GROUP_TOKEN);

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
