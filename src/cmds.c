#include "vkapi.h"
#include "va_utils.h"
#include <string.h>
#include <strings.h>
#include <time.h>
#include <limits.h>
#include <crc_hash.h>
#include <curl/curl.h>
#include "curl_wrap.h"
#include "base64.h"
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

  while (ptr != NULL) {
      char *str = va( "• %s - %s\n", ptr->string, ptr->description );
      string_strncat( s, str, strlen( str ) );
      ptr = ptr->next;
    }

  vkapi_send_message( object, message->peer_id, s->ptr, NULL, 0 );
  string_destroy( s );
}

void cmd_about_bot(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();

  string_format( s,"VKBot\nБот написан на C\nИспользует библиотеки libcurl и cjson\nСобран %s %s\nВерсия %s", __DATE__, __TIME__, VERSION );

  vkapi_send_message(object, message->peer_id, s->ptr, NULL, 0);

  string_destroy(s);
}

void cmd_ping(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message( object, message->peer_id, "Pong", NULL, 0 );
}

void cmd_base64_encode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message(object, message->peer_id, "Использование: b64e <строка>", NULL, 0);
      return;
    }

  char *base64_encoded_string = (char*)base64_encode( (const unsigned char *)args, strlen( args ), NULL);
  if(base64_encoded_string)
    {
      vkapi_send_message( object, message->peer_id, va("Закодированная строка: %s", base64_encoded_string ), NULL, 0 );
      free(base64_encoded_string);
    }
}

int base64_string(const char *base64_str);

void cmd_base64_decode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: b64d <строка>", NULL, 0 );
      return;
    }

  if( !base64_string(args) )
    {
      vkapi_send_message( object, message->peer_id, "Строка имеет недопустимые символы для base64", NULL, 0 );
      return;
    }

  char *base64_decoded_string = (char*)base64_decode( (const unsigned char *)args, strlen( args ), NULL );
  if(base64_decoded_string)
    {
      vkapi_send_message( object, message->peer_id, va( "Декодированная строка: %s", base64_decoded_string ), NULL, 0 );
      free(base64_decoded_string);
    }
}

int worker_get_workers_count(void);
size_t worker_commands_processed(void);
size_t worker_message_processed(void);

long get_param_of_path(const char *filenpath, const char *what);

void cmd_stat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_format( s, "Статистика бота\nКоличество работающих воркеров: %i\nПик очереди: %lu\nПамяти сожрано мной: %ld кб\nМаксимальный размер кеша: %lu кб\nСъел кеш: %lu кб\nКомманд обработано: %lu\nСообщений обработано: %lu\n", worker_get_workers_count(), queue_maxium_tasks(), get_param_of_path( "/proc/self/status", "VmRSS" ),memcache_get_max_size(), memcache_get_size() / 1024, worker_commands_processed(), worker_message_processed() );
  vkapi_send_message( object, message->peer_id, s->ptr, NULL, 0 );
  string_destroy( s );
}

int64_t random_int64(int64_t min, int64_t max)
{
  if(min > max)
    return 0;

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
      vkapi_send_message( object, message->peer_id, "Использование: ранд максимальное число\nранд минимальное число максимальное число\n", NULL, 0 );
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
      vkapi_send_message( object, message->peer_id, va("Случайное число: %lli", random_int64( atoll( argv[1] ), atoll( argv[2] ) ) ), NULL, 0 );
    }
  else {
      vkapi_send_message( object, message->peer_id, va("Случайное число: %lli", random_int64( 1, atoll( argv[1] ) ) ), NULL, 0 );
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

  vkapi_send_message( object, message->peer_id, s->ptr, NULL, 0 );
  string_destroy(s);
}

void cmd_who(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
//    string_t method_args = string_init();
//    string_format(method_args, "peer_id=%i", message->peer_id);
//    string_t s = vkapi_call_method(object, "messages.getConversationMembers", method_args, true);
//    vkapi_send_message(object, message->peer_id, s->ptr);
}

void cmd_flip(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  int64_t random = random_int64( 0, 10000 );

  if( random >= 5000 )
    {
      vkapi_send_message( object, message->peer_id, "Орёл!", NULL, 0 );
    } else {
      vkapi_send_message( object, message->peer_id, "Решка!", NULL, 0 );
   }
}

void cmd_info(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: инфа <строка>", NULL, 0 );
      return;
    }

  int64_t percents = random_int64( 0, 172 );

  vkapi_send_message( object, message->peer_id, va("Вероятность этого составляет %lli%%", percents), NULL, 0 );
}

void cmd_rate(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        vkapi_send_message( object, message->peer_id, "Использование: оцени <строка>", NULL, 0 );
        return;
      }

    int64_t rate = random_int64( 0, 11 );

    vkapi_send_message( object, message->peer_id, va("Я оцениваяю %s в %lli/10", args, rate), NULL, 0 );
}

void cmd_rand_docs(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        vkapi_send_message( object, message->peer_id, "Использование: доки <строка для поиска>", NULL, 0 );
        return;
      }

    string_t s = string_init();

    string_format(s, "q=%s&count=10", args);

    string_t result = vkapi_call_method( object, "docs.search", s, true);

    cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

    cJSON *resp = cJSON_GetObjectItem(ptr, "response");

    printf("HMM: %s\n", result->ptr);

    cJSON *count = cJSON_GetObjectItem(resp, "count");

    if(!count->valueint)
    {
       cJSON_Delete(ptr);

       vkapi_send_message(object, message->peer_id, "По вашему запросу нету документов", NULL, 0);

       string_destroy(s);
       string_destroy(result);
       return;
    }

    cJSON *items = cJSON_GetObjectItem(resp, "items");

    int docs_count = cJSON_GetArraySize(items);

    vkapi_attach *attaches = (vkapi_attach*)calloc(docs_count, sizeof(vkapi_attach));

    for(int i = 0; i < docs_count; i++)
    {
        cJSON *obj = cJSON_GetArrayItem(items, i);

        attaches[i].type = VKAPI_DOC;
        attaches[i].media_id = cJSON_GetObjectItem(obj, "id")->valueint;
        attaches[i].owner_id = cJSON_GetObjectItem(obj, "owner_id")->valueint;
    }

    vkapi_send_message(object, message->peer_id, "Документы по вашему запросу:", attaches, docs_count);

    cJSON_Delete(ptr);

    string_destroy(s);
    string_destroy(result);
}

void cmd_valute_curse(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args )
{

    if(argc > 2 || argc < 1)
    {
        vkapi_send_message(object, message->peer_id, "Для того чтобы узнать какие курсы валют доступны используйте курс помощь", NULL, 0);
        return;
    }

    if(!strcasecmp("помощь", argv[1]))
    {
        string_t s = string_init();
        string_t s2 = string_init();

        string_format(s2, "https://www.cbr-xml-daily.ru/daily_json.js" );

        if(!curl_get(object->curl_handle, s2, NULL, s ))
        {
            string_destroy(s);
            string_destroy(s2);

            vkapi_send_message(object, message->peer_id, "Oops", NULL, 0);
            return;
        }

        string_destroy(s2);

        s2 = string_init();

        char *info = va( "• Код - Название валюты\n" );

        string_strncat( s2, info, strlen(info) );

        cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false), *valute_element = NULL, *array_value = NULL;

        array_value = cJSON_GetObjectItem(ptr, "Valute");

        cJSON_ArrayForEach( valute_element, array_value )
        {

            cJSON *CharCode = cJSON_GetObjectItem(valute_element, "CharCode");
            cJSON *Name = cJSON_GetObjectItem(valute_element, "Name");

            info = va("• %s - %s\n", cJSON_GetStringValue(CharCode), cJSON_GetStringValue(Name));

            string_strncat(s2, info, strlen(info));
        }

        vkapi_send_message(object, message->peer_id, s2->ptr, NULL, 0);

        cJSON_Delete(ptr);
        string_destroy(s);
        string_destroy(s2);
        return;
    }

    string_t s = string_init();
    string_t s2 = string_init();

    string_format(s2, "https://www.cbr-xml-daily.ru/daily_json.js" );

    if(!curl_get(object->curl_handle, s2, NULL, s ))
    {
        string_destroy(s);
        string_destroy(s2);

        vkapi_send_message(object, message->peer_id, "Oops", NULL, 0);
        return;
    }

    string_destroy(s2);

    s2 = string_init();

    cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false), *valute = NULL, *array_value = NULL;

    array_value = cJSON_GetObjectItem(ptr, "Valute");

    valute = cJSON_GetObjectItem(array_value, argv[1]);

    if(!valute)
    {
        vkapi_send_message(object, message->peer_id, "Валюты с таким кодом нету!", NULL, 0);

        cJSON_Delete(ptr);
        string_destroy(s);
        string_destroy(s2);
        return;
    }

    cJSON *Value = cJSON_GetObjectItem(valute, "Value");
    cJSON *Previous = cJSON_GetObjectItem(valute, "Previous");
    cJSON *Name = cJSON_GetObjectItem(valute, "Name");

    double diff = Value->valuedouble - Previous->valuedouble;

    char *info = NULL;

    if(diff < 0.0)
    {
    info = va("Курс валют\n%s: %f\n↓ %f\n", cJSON_GetStringValue(Name), Value->valuedouble, diff );
    } else {
    info = va("Курс валют\n%s: %f\n↑ %f\n", cJSON_GetStringValue(Name), Value->valuedouble, diff );
    }

    string_strncat(s2, info, strlen(info));

    vkapi_send_message(object, message->peer_id, s2->ptr, NULL, 0);

    cJSON_Delete(ptr);
    string_destroy(s);
    string_destroy(s2);
}

#define OPEN_WEATHER_TOKEN "4a223e7db4e8c28265df2633448b69f3"
#define OPEN_WEATHER_URL "https://api.openweathermap.org"
#define OPEN_WEATHER_METHOD "data/2.5/weather"
#define OPEN_WEATHER_LANG "ru"

size_t curl_dynamic_string_writefunc(void *ptr, size_t size, size_t nmemb, void *data);

void cmd_weather(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: погода <Город>", NULL, 0 );
      return;
    }

  string_t url_get = string_init();
  string_t openweather_json = string_init();

  string_format(url_get, "%s/%s?q=%s&appid=%s&lang=%s&units=metric", OPEN_WEATHER_URL, OPEN_WEATHER_METHOD, argv[1], OPEN_WEATHER_TOKEN, OPEN_WEATHER_LANG );

  curl_easy_reset(object->curl_handle);
  curl_easy_setopt(object->curl_handle, CURLOPT_URL, url_get->ptr);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, curl_dynamic_string_writefunc);
  curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, openweather_json);

  CURLcode error_code = curl_easy_perform(object->curl_handle);

  string_destroy(url_get);

  if(error_code != CURLE_OK)
    {
      vkapi_send_message(object, message->peer_id, "Произошла неизвестная ошибка", NULL, 0);
      string_destroy(openweather_json);
      return;
    }

    if(argv[argc])
      {
    if(!strncasecmp(argv[argc], "json", 4))
      {
    vkapi_send_message(object, message->peer_id, openweather_json->ptr, NULL, 0);
	string_destroy(openweather_json);
	return;
      }
      }

  cJSON *ptr = cJSON_ParseWithOpts(openweather_json->ptr, NULL, false);

  if(!ptr)
    {
    vkapi_send_message(object, message->peer_id, "cJSON сделал рыг рыг пук пук", NULL, 0);
	string_destroy(openweather_json);
	return;
    }

  cJSON *cod = cJSON_GetObjectItem(ptr, "cod");


        if(cod->valueint == 200)
        {
	  string_t msg = string_init();

	  cJSON *weather_array = cJSON_GetObjectItem(ptr, "weather");
	  cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
	  cJSON *name = cJSON_GetObjectItem(ptr, "name");
	  cJSON *main_obj = cJSON_GetObjectItem(ptr, "main");
	  cJSON *temp = cJSON_GetObjectItem(main_obj, "temp");

	  string_format(msg, "Погода в %s\n\n• Сейчас: %i℃, %s\n", cJSON_GetStringValue(name), temp->valueint, cJSON_GetStringValue(cJSON_GetObjectItem(weather, "description")));

      vkapi_send_message(object, message->peer_id, msg->ptr, NULL, 0);

	  string_destroy(msg);
	  cJSON_Delete(ptr);
	  string_destroy(openweather_json);
	  return;
        } else if(!strncmp(cod->valuestring, "404", 3))
	{
      vkapi_send_message(object, message->peer_id, "Такого города нет", NULL, 0);
	  cJSON_Delete(ptr);
	  string_destroy(openweather_json);
	  return;
	} else if(!strncmp(cod->valuestring, "401", 3)) {
      vkapi_send_message(object, message->peer_id, "Ограничение апи openweathermap", NULL, 0);
	  cJSON_Delete(ptr);
	  string_destroy(openweather_json);
	  return;
	} else {
      vkapi_send_message(object, message->peer_id, "Неизвестная ошибка", NULL, 0);
	  cJSON_Delete(ptr);
	  string_destroy(openweather_json);
	  return;
	}
}

void cmd_crc32(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
   string_t msg = string_init();
   string_t file = string_init();

   if(argc < 1 && !message->attachmens)
    {
      usage:
      vkapi_send_message(object, message->peer_id, "Использование: crc32 <строка> или прикреплённый документ", NULL, 0);
      string_destroy(msg);
      string_destroy(file);
      return;
    }

  uint32_t crc32 = crc32_calc((const unsigned char *)args, strlen(args));

  if(argc > 0)
  vkapi_send_message(object, message->peer_id, va("CRC32 хеш строки: 0x%X", crc32), NULL, 0);
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
      curl_easy_setopt(object->curl_handle, CURLOPT_WRITEFUNCTION, curl_dynamic_string_writefunc);
      curl_easy_setopt(object->curl_handle, CURLOPT_WRITEDATA, file);

      curl_easy_perform(object->curl_handle);

      char *ptr = va("CRC32 хеш файла %s: 0x%X\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), crc32_calc((const unsigned char *)file->ptr, file->len));

      string_strncat(msg, ptr, strlen(ptr));

      string_destroy(file);
      }

      vkapi_send_message(object, message->peer_id, msg->ptr, NULL, 0);
      string_destroy(msg);
    }
}

void cmd_hlmemes(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message(object, message->peer_id, "Держи: https://www.youtube.com/playlist?list=PLxdH4XPU7OZnR5XV-b1QmnM7yCwAlmOUd", NULL, 0);
}

void cmd_cat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_t url = string_init();

  string_copy(url, "https://aws.random.cat/meow");

  if(curl_get(object->curl_handle, url, NULL, s) != true)
    {
      vkapi_send_message(object, message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      string_destroy(s);
      string_destroy(url);
      return;
    }

  cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false);

  if(!ptr)
    {
      vkapi_send_message(object, message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      string_destroy(s);
      string_destroy(url);
      return;
    }

  cJSON *file = cJSON_GetObjectItem(ptr, "file");

  string_copy(url, cJSON_GetStringValue(file));

  string_t filedata = string_init();

  curl_get(object->curl_handle, url, NULL, filedata);

  vkapi_attach *attach = vkapi_upload_doc_by_url(object, message, "random_cat.png", filedata, VKAPI_PHOTO);

  vkapi_send_message(object, message->peer_id, NULL, attach, 1);

  cJSON_Delete(ptr);

  string_destroy(s);
  string_destroy(filedata);
  string_destroy(url);
}

#include "users.h"

void cmd_debug(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
    privilage_t priv = get_privilage_by_id(message->from_id);

    string_t s = string_init();

    string_format(s, "Ваш уровень привелегий - %s\n", get_name_privilage(priv));

    vkapi_send_message(object, message->peer_id, s->ptr, NULL, 0);

    string_destroy(s);

    return;

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
      vkapi_send_message(object, message->peer_id, va("Memcache: push key \"%s\" value \"%s\"\n", argv[1], s->ptr), NULL, 0);
	  return;
	}
      const char *value = memcache_get(argv[1]);
      if(value)
      vkapi_send_message(object, message->peer_id, va("Memcache: key is \"%s\" value is \"%s\"\n", argv[1], value), NULL, 0);
      else
    vkapi_send_message(object, message->peer_id, va("Memcache: value for key \"%s\" not found\n", argv[1]), NULL, 0);
    }
}
