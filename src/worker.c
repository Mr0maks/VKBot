/*
worker.c - VK longpoll workers
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "common.h"
#include "config.h"
#include <pthread.h>

//static pthread_mutex_t queue_lock;
//static pthread_cond_t queue_cond_var;

static threadpool worker_pool = NULL;

static volatile size_t command_processed = 0;
static volatile size_t message_processed = 0;

//static pthread_t longpoll_thread = 0;

typedef struct
{
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
#ifdef VKBOT_FIND_LEAK
    CHECK_LEAKS();
#endif
    cJSON_Delete(json_event);
}

static vkapi_handle *longpoll_vkapi_object = NULL;

void longpoll_worker( void )
{
      string_t long_poll_string = vkapi_get_longpoll_data( longpoll_vkapi_object );

      assert( long_poll_string );

      if(config.debug == true)
          Con_Printf( "%s\n", long_poll_string->ptr );

      cJSON *main_obj = cJSON_ParseWithOpts( long_poll_string->ptr, NULL, false );

      if( !main_obj )
      {
          Con_Printf( "Error while getting long poll data: json parser return NULL\n");
          string_destroy( long_poll_string );
          return;
      }

      cJSON *failed = cJSON_GetObjectItem(main_obj, "failed");

      if(failed != NULL)
      {
          switch (failed->valueint) {
          // lost events history or events history outdate
          case 1:
          {
              cJSON *ts = cJSON_GetObjectItem(main_obj, "ts");
              assert(ts);
              // VK FUCK YOU
              longpoll_vkapi_object->longpoll_timestamp = ts->valueint;
              cJSON_Delete(main_obj);
              string_destroy(long_poll_string);
              return;
          }
          // longpoll key outdate
          case 2:
          // lost key and ts
          case 3:
          default:
          {
              cJSON_Delete( main_obj );
              string_destroy( long_poll_string );
              vkapi_get_long_poll_server( longpoll_vkapi_object );
              return;
          }
          }
      } else {
          cJSON *ts = cJSON_GetObjectItem(main_obj, "ts");
          longpoll_vkapi_object->longpoll_timestamp = atoll(cJSON_GetStringValue(ts));
      }

      if( !vkapi_json_long_poll_have_updates( main_obj ) )
      {
          cJSON_Delete( main_obj );
          string_destroy( long_poll_string );
          return;
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

  //pthread_mutex_init( &queue_lock, NULL );
  //pthread_cond_init( &queue_cond_var, NULL );

  work_data = calloc(num_workers + 1, sizeof(worker_data_t));

  worker_pool = thpool_init( num_workers + 1 );

  for( int i = 0; i < num_workers + 1; i++ )
    {
      work_data[i].vkapi_object = vkapi_init( config.token );
    }

    longpoll_vkapi_object = vkapi_init( config.token );

try_again:
    if(!vkapi_get_long_poll_server(longpoll_vkapi_object))
    {
        Con_Printf("Error while getting long poll server. I try again later.\n");
        sleep(10);
        goto try_again;
    }
}

void worker_deinit(void)
{
    thpool_destroy( worker_pool );
    for( int i = 0; i < config.num_workers + 1; i++ )
    {
        vkapi_destroy(work_data[i].vkapi_object);
    }
    vkapi_destroy(longpoll_vkapi_object);
    free(work_data);
}
