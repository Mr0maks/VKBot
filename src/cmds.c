#include "vkapi.h"
#include "va_utils.h"
#include <string.h>
#include <strings.h>
#include <time.h>
#include <limits.h>
#include <crc32_hash.h>

#include <curl/curl.h>

#include "base64_decoder.h"

#include "worker_queue.h"

#include "cmd_handler.h"

#include "memcache.h"

extern cmds_t commands[];

extern cmds_modules_pools_t *modules_cmds_poll;

void cmd_help(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();

  for(int i = 0; commands[i].string != NULL; i++ )
    {
      char *str = va( "• %s - %s\n", commands[i].string, commands[i].description );
      string_strncat( s, str, strlen( str ) );
    }

  cmds_modules_pools_t *ptr = modules_cmds_poll;

  while (ptr) {
      char *str = va( "• %s - %s\n", ptr->string, ptr->description );
      string_strncat( s, str, strlen( str ) );
      ptr = ptr->next;
    }

  vkapi_send_message( object, message->peer_id, s->ptr );
  string_destroy( s );
}

void cmd_about_bot(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();

  string_format( s,"VKBot\nИспользует библиотеки libcurl и cjson\nСобран %s %s\nВерсия %s", __DATE__, __TIME__, VERSION );

  vkapi_send_message(object, message->peer_id, s->ptr);

  string_destroy(s);
}

void cmd_ping(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message( object, message->peer_id, "Pong" );
}

void cmd_base64_encode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = NULL;

  if( argc < 1 )
    {
      vkapi_send_message(object, message->peer_id, "Использование: b64e <строка>");
      return;
    }

  s = string_init();
  base64_encode( (const unsigned char *)args, (unsigned char *)s->ptr, strlen( args ), 0);
  vkapi_send_message( object, message->peer_id, va("Закодированная строка: %s", s->ptr ) );
  string_destroy( s );
}

int base64_string(const char *base64_str);

void cmd_base64_decode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = NULL;

  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: b64d <строка>" );
      return;
    }

  if( !base64_string(args) )
    {
      vkapi_send_message( object, message->peer_id, "Строка имеет недопустимые символы для base64" );
      return;
    }

  s = string_init();
  base64_decode( (const unsigned char *)args, (unsigned char *)s->ptr, strlen( args ) );
  vkapi_send_message( object, message->peer_id, va( "Декодированная строка: %s", s->ptr ) );
  string_destroy( s );
}

int worker_get_workers_count(void);
size_t worker_commands_processed(void);
size_t worker_message_processed(void);

long get_param_of_path(const char *filenpath, const char *what);

void cmd_stat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_format( s, "Статистика бота\nКоличество работающих воркеров: %i\nПик очереди: %lu\nПамяти сожрано мной: %ld кб\nМаксимальный размер кеша: %lu кб\nСъел кеш: %lu кб\nКомманд обработано: %lu\nСообщений обработано: %lu\n", worker_get_workers_count(), queue_maxium_tasks(), get_param_of_path( "/proc/self/status", "VmRSS" ),memcache_get_max_size(), memcache_get_size() / 1024, worker_commands_processed(), worker_message_processed() );
  vkapi_send_message( object, message->peer_id, s->ptr );
  string_destroy( s );
}

int64_t random_int64(int64_t min, int64_t max)
{
  if(min != LLONG_MIN || max != LLONG_MAX)
  return min + rand() % ( max + 1 - min);
  else
  return 0;
}

time_t random_time(time_t min, time_t max)
{
  return min + rand() % ( max + 1 - min );
}

void cmd_rand(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 || argc > 2 )
    {
      usage:
      vkapi_send_message( object, message->peer_id, "Использование: ранд максимальное число\nранд минимальное число максимальное число\n" );
      return;
    }

  if( argc > 1 && !atoll( argv[2] ) )
    {
      goto usage;
    }

  if( argc == 1 && !atoll( argv[1] ) )
    {
      goto usage;
    }

  if( argc == 2 )
    {
      vkapi_send_message( object, message->peer_id, va("Случайное число: %lli", random_int64( atoll( argv[1] ), atoll( argv[2] ) ) ) );
    }
  else {
      vkapi_send_message( object, message->peer_id, va("Случайное число: %lli", random_int64( 1, atoll( argv[1] ) ) ) );
    }

}

void cmd_rand_date(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  unsigned int seed = UINT_MAX;
  string_t s = string_init();
  time_t date = 0;

  if(argv[1])
    {
    seed = crc32_calc( (const unsigned char *)args, strlen(args) );
    date = random_time( time(NULL), seed );
    } else {
     date = random_time( time(NULL), seed );
    }

  strftime(s->ptr, s->size, "Это произойдёт %e %B %Y", localtime( &date ));

  vkapi_send_message( object, message->peer_id, s->ptr );
  string_destroy(s);
}

void cmd_who(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t method_args = string_init();

  string_format(method_args, "peer_id=%i", message->peer_id);

  string_t s = vkapi_call_method(object, "messages.getConversationMembers", method_args, true);

  vkapi_send_message(object, message->peer_id, s->ptr);
}

void cmd_flip(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  int64_t random = random_int64( 0, 10000 );

  if( random >= 5000 )
    {
      vkapi_send_message( object, message->peer_id, "Орёл!" );
    } else {
      vkapi_send_message( object, message->peer_id, "Решка!" );
   }
}

void cmd_info(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: инфа <строка>" );
    }

  int64_t percents = random_int64( 0, 197 );

  vkapi_send_message( object, message->peer_id, va("Вероятность этого составляет %lli%%", percents) );
}

#define OPEN_WEATHER_TOKEN "4a223e7db4e8c28265df2633448b69f3"
#define OPEN_WEATHER_URL "https://api.openweathermap.org"
#define OPEN_WEATHER_METHOD "data/2.5/weather"
#define OPEN_WEATHER_LANG "ru"

size_t vk_string_writefunc(void *ptr, size_t size, size_t nmemb, void *data);

void cmd_weather(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: погода <Город>" );
    }

  string_t params_to_opwe = string_init();

  string_t s = string_init();

  string_format(params_to_opwe, "%s/%s?q=%s&appid=%s&lang=%s&units=metric", OPEN_WEATHER_URL, OPEN_WEATHER_METHOD, argv[1], OPEN_WEATHER_TOKEN, OPEN_WEATHER_LANG );

  curl_easy_setopt(object->curl_handle, CURLOPT_URL, params_to_opwe->ptr);

  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, vk_string_writefunc);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, s);

  CURLcode error_code = curl_easy_perform(object->curl_handle);

  if(error_code != CURLE_OK)
    {
      vkapi_send_message(object, message->peer_id, "Произошла неизвестная ошибка");
      string_destroy(s);
      string_destroy(params_to_opwe);
      return;
    }

    string_destroy(params_to_opwe);

    if(argv[argc])
      {
    if(!strncasecmp(argv[argc], "json", 4))
      {
	vkapi_send_message(object, message->peer_id, s->ptr);
	string_destroy(s);
	return;
      }
      }

  cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false);

  if(!ptr)
    {
	vkapi_send_message(object, message->peer_id, "cJSON сделал рыг рыг пук пук");
	return;
    }

  cJSON *cod = cJSON_GetObjectItem(ptr, "cod");

      if(cod->valueint == 404)
	{
	  vkapi_send_message(object, message->peer_id, "Такого города нет");
	  cJSON_Delete(ptr);
	  string_destroy(s);
	  return;
	} else if(cod->valueint == 401) {
	  vkapi_send_message(object, message->peer_id, "Ограничение апи openweathermap");
	  cJSON_Delete(ptr);
	  string_destroy(s);
	  return;
	}

  string_t msg = string_init();

  cJSON *weather_array = cJSON_GetObjectItem(ptr, "weather");
  cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
  cJSON *name = cJSON_GetObjectItem(ptr, "name");
  cJSON *main = cJSON_GetObjectItem(ptr, "main");
  cJSON *temp = cJSON_GetObjectItem(main, "temp");

  string_format(msg, "Погода в %s\n\n• Сейчас: %i℃, %s\n", cJSON_GetStringValue(name), temp->valueint, cJSON_GetStringValue(cJSON_GetObjectItem(weather, "description")));

  vkapi_send_message(object, message->peer_id, msg->ptr);

  string_destroy(msg);
  cJSON_Delete(ptr);
  string_destroy(s);
}

void cmd_crc32(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
   string_t msg = string_init();
   string_t file = string_init();

   if(argc < 1 && !message->attachmens)
    {
      usage:
      vkapi_send_message(object, message->peer_id, "Использование: crc32 <строка> или прикреплённый документ");
      string_destroy(msg);
      string_destroy(file);
      return;
    }

  uint32_t crc32 = crc32_calc((const unsigned char *)args, strlen(args));

  if(argc > 0)
  vkapi_send_message(object, message->peer_id, va("CRC32 хеш строки: 0x%X", crc32));
  else
    {

      cJSON *attach = NULL;

      cJSON_ArrayForEach(attach, message->attachmens)
      {

      cJSON *doc = cJSON_GetObjectItem(attach, "doc");

      if(!doc)
	{
	string_destroy(file);
	goto usage;
	}

      curl_easy_setopt(object->curl_handle, CURLOPT_URL, cJSON_GetStringValue(cJSON_GetObjectItem(doc, "url")));
      curl_easy_setopt(object->curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
      //curl_easy_setopt(object->curl_handle, CURLOPT_VERBOSE, 1L);
      curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, vk_string_writefunc);
      curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, file);

      curl_easy_perform(object->curl_handle);

      char *ptr = va("CRC32 хеш файла %s: 0x%X\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), crc32_calc((const unsigned char *)file->ptr, file->len));

      string_strncat(msg, ptr, strlen(ptr));

      string_destroy(file);
      }

      vkapi_send_message(object, message->peer_id, msg->ptr);
      string_destroy(msg);
    }
}

#include "users.h"

void cmd_debug(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  //vkapi_send_message(object, message->peer_id, va("Ваш уровень прав: %s\n", get_name_privilage(get_privilage_by_id(message->from_id))));

  if(argv[1])
    {
      if(argv[2])
	{
	  string_t s = string_init();

	      for( int c = 2; c < argc + 1; c++ )
		{
		  if( !argv[c] )
		    break;

		  if(c > 2)
		    string_strncat( s, " ", 1 );

		  string_strncat( s, argv[c], strlen(argv[c]) );
		}

	  memcache_push(argv[1], s->ptr);
	  vkapi_send_message(object, message->peer_id, va("Memcache: push key \"%s\" value \"%s\"\n", argv[1], s->ptr));
	  return;
	}
      const char *value = memcache_get(argv[1]);
      if(value)
      vkapi_send_message(object, message->peer_id, va("Memcache: key is \"%s\" value is \"%s\"\n", argv[1], value));
      else
	vkapi_send_message(object, message->peer_id, va("Memcache: value for key \"%s\" not found\n", argv[1]));
    }
}
