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

static pthread_mutex_t mutex_lock;
static pthread_cond_t cond_var;

static threadpool worker_pool = NULL;

void long_poll_worker( void *data )
{
  (void)data;

  vkapi_object *vkapi_object = vk_api_init( VK_GROUP_TOKEN, VK_GROUP_ID );

  while (1) {
      start:

	sleep( 1 );

      pthread_mutex_lock( &mutex_lock );

      while( queue_is_empty() )
	{
	pthread_cond_wait( &cond_var, &mutex_lock );
	}

      vkapi_message_new_object *message = get_queue();

      pthread_mutex_unlock( &mutex_lock );

      if( message == NULL )
	goto start;

	 if( vkapi_object->group_id == (message->from_id * -1) )
	   {
	     if( message->attachmens )
	       cJSON_Delete( message->attachmens );

	     string_destroy( message->text );

	     free( message );
	    continue;
	   }

	 struct timeval tv;
	 gettimeofday( &tv, NULL );
	 long long int ms = tv.tv_sec*1000LL + tv.tv_usec/1000;

      printf( "Message from %i ot %i : %s\n", message->peer_id, message->from_id, message->text->ptr );

      if( message->text->len < 256 )
	{
	  if(cmd_handle( vkapi_object, message ))
	    {
	      struct timeval tv2;
	      gettimeofday( &tv2, NULL );
	      long long int ms2 = tv2.tv_sec*1000LL + tv2.tv_usec/1000;
	      vkapi_send_message( vkapi_object , message->peer_id, va("Сообщение обработано за %lli мс", ms2 - ms ));
	    }
	}

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

void worker_main_thread( const char *token, const char *group_id, int num_workers )
{
  queue_init();
  cmd_handler_init();

  pthread_mutex_init( &mutex_lock, NULL);
  pthread_cond_init( &cond_var, NULL);

  worker_pool = thpool_init( num_workers );

  for( int i = 0; i < num_workers; i++ )
    {
      thpool_add_work( worker_pool, (void *)long_poll_worker, NULL );
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

      long_poll_string = vkapi_get_longpoll_data( object );

      if( long_poll_string == NULL )
	goto try_again;

      cJSON *main_obj = cJSON_ParseWithOpts( long_poll_string->ptr, NULL, false );

      printf( "%s\n", long_poll_string->ptr );

      if( !json_vkapi_long_poll_have_updates( main_obj ) )
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

	add_queue( copy );

      }

      pthread_mutex_unlock( &mutex_lock );
      pthread_cond_broadcast( &cond_var );

      cJSON_Delete( main_obj );
      string_destroy( long_poll_string );
    }

  thpool_wait( worker_pool );
}
