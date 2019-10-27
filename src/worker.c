#include "common.h"
#include "console.h"
#include "config.h"

static pthread_mutex_t mutex_lock;
static pthread_mutex_t command_handler_mutex;
static pthread_cond_t cond_var;

static threadpool worker_pool = NULL;

static volatile size_t command_processed;
static volatile size_t message_processed;

static volatile bool main_thread_loop = false;

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
  worker_data_t *worker_data = (worker_data_t*)data;

  Con_Printf("[Worker %i] OK!\n", worker_get_worker_id());

  while (worker_data->loop) {

      sleep( 1 );

      cJSON *json_event = NULL;

      pthread_mutex_lock( &mutex_lock );

      while( json_event == NULL )
	{
	pthread_cond_wait( &cond_var, &mutex_lock );
    json_event = queue_pop();
	}

      pthread_mutex_unlock( &mutex_lock );

      assert(json_event);

      events_manager(json_event);

      CHECK_LEAKS();
    }
}

void longpool_worker( void *data )
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

      if(config.debug_workers)
          Con_Printf( "%s\n", long_poll_string->ptr );

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

void load_modules(void);

void worker_main_thread( const char *token, int num_workers )
{
    if(!token)
    {
        Con_Printf("Error: no token from config file!\n");
        return;
    }

    if(!num_workers)
    {
        Con_Printf("Warn: no workers count from config file!\n");

        num_workers = 4;
    }

  load_modules();

  GC_INIT()

  GC_enable_incremental();

  if(config.gc_disable)
      GC_disable();

  queue_init();
  cmd_handler_init();
  memcache_init(64);

  command_processed = 0;
  message_processed = 0;

  main_thread_loop = true;

  pthread_mutex_init( &mutex_lock, NULL );
  pthread_mutex_init( &command_handler_mutex, NULL );
  pthread_cond_init( &cond_var, NULL );

  work_data = calloc(num_workers + 2, sizeof(worker_data_t));

  worker_pool = thpool_init( num_workers + 2 );

  for( int i = 0; i < num_workers + 2; i++ )
    {
      work_data[i].loop = true;
      work_data[i].vkapi_object = vkapi_init( token );
    }

  for( int i = 0; i < num_workers; i++ )
    {
      thpool_add_work( worker_pool, lp_event_worker, &work_data[i] );
      //HACKHACK: fast worker init break worker_get_id (race ?) :(
      usleep(500);
    }

  thpool_add_work( worker_pool, longpool_worker, &work_data[num_workers + 1] );

  char buff[256];

  while (main_thread_loop) {
      while(fgets(buff, sizeof (buff), stdin) == NULL) { }
      console_handler(buff);
  }

  thpool_pause( worker_pool );
  thpool_destroy( worker_pool );
  queue_deinit();
  cmd_handler_deinit();

  for( int i = 0; i < num_workers + 2; i++ )
    {
      vkapi_destroy(work_data[i].vkapi_object);
    }

  free(work_data);
}
