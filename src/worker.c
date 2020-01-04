#include "common.h"
#include "console.h"
#include "config.h"
#include <pthread.h>

static pthread_mutex_t queue_lock;
static pthread_cond_t queue_cond_var;

static threadpool worker_pool = NULL;

static volatile size_t command_processed;
static volatile size_t message_processed;

static pthread_t longpool_thread = 0;

typedef struct
{
  bool loop;
  vkapi_handle *vkapi_object;
} worker_data_t;

static worker_data_t *work_data = NULL;

int worker_get_worker_id(void) {
    int id = thpool_get_thread_id(worker_pool);
    return id;
}

vkapi_handle *worker_get_vkapi_handle()
{
    int id = worker_get_worker_id();
    return work_data[id].vkapi_object;
}

void lp_event_worker( void *data )
{
    cJSON *json_event = (cJSON*)data;
    assert(json_event);
    events_manager(json_event);
    CHECK_LEAKS();
}

void *longpool_worker( void *data )
{
    worker_data_t *worker_data = (worker_data_t*)data;
    vkapi_handle *vkapi_object = worker_data->vkapi_object;

try_again:
    if(!vkapi_get_long_poll_server(vkapi_object))
    {
      Con_Printf("Error while getting long poll server. I try again.\n");
      sleep(10);
      goto try_again;
    }

  while (worker_data->loop) {
      string_t long_poll_string = NULL;

      long_poll_string = vkapi_get_longpoll_data( vkapi_object );

      if( long_poll_string == NULL )
          goto try_again;

      cJSON *main_obj = cJSON_ParseWithOpts( long_poll_string->ptr, NULL, false );

//      if(config.debug_workers)
          Con_Printf( "%s\n", long_poll_string->ptr );

      if( !vkapi_json_long_poll_have_updates( main_obj ) )
      {
          cJSON_Delete( main_obj );
          string_destroy( long_poll_string );
          continue;
      }

      cJSON *updates = cJSON_GetObjectItem( main_obj, "updates" );
      cJSON *update_block = NULL;

      cJSON_ArrayForEach( update_block, updates )
      {
          cJSON *copy = cJSON_Duplicate( update_block, true );

          if( !copy )
              break;

          thpool_add_work(worker_pool, lp_event_worker, copy);
      }

      cJSON_Delete( main_obj );
      string_destroy( long_poll_string );
    }
  return NULL;
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

void worker_init(void)
{
    if(!config.token)
    {
        Con_Printf("Error: no token from config file!\n");
        return;
    }

    if(!config.num_workers)
    {
        Con_Printf("Warn: no workers count from config file!\n");

        config.num_workers = 4;
    }

    int num_workers = config.num_workers;

  command_processed = 0;
  message_processed = 0;

  pthread_mutex_init( &queue_lock, NULL );
  pthread_cond_init( &queue_cond_var, NULL );

  work_data = calloc(num_workers + 2, sizeof(worker_data_t));

  worker_pool = thpool_init( num_workers + 1 );

  for( int i = 0; i < num_workers + 2; i++ )
    {
      work_data[i].loop = true;
      work_data[i].vkapi_object = vkapi_init( config.token );
    }

  pthread_create(&longpool_thread, NULL, longpool_worker, &work_data[num_workers + 1]);
  pthread_detach(longpool_thread);
  //thpool_add_work( worker_pool, longpool_worker, &work_data[num_workers + 1] );
}

void worker_deinit(void)
{
    for( int i = 0; i < config.num_workers + 2; i++ )
      {
        work_data[i].loop = false;
      }

    thpool_destroy( worker_pool );

    pthread_join(longpool_thread, NULL);

    for( int i = 0; i < config.num_workers + 2; i++ )
      {
        work_data[i].loop = false;
        vkapi_destroy(work_data[i].vkapi_object);
      }

    free(work_data);
}
