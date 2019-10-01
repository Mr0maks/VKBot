#include "va_utils.h"
#include <string.h>
#include <strings.h>
#include <time.h>
#include <limits.h>
#include <curl/curl.h>
#include "base64.h"
#include "cmd_handler.h"
#include "enginecallbacks.h"

//FIXME: MAKE WRAPPER AROUND CJSON
#include <cJSON.h>

void cmd_ping(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  VKAPI_SEND_MESSAGE( object, message->peer_id, "Pong", NULL, 0 );
}

void cmd_base64_encode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Использование: b64e <строка>", NULL, 0);
      return;
    }

  char *base64_encoded_string = (char*)base64_encode( (const unsigned char *)args, strlen( args ), NULL);
  if(base64_encoded_string)
    {
      VKAPI_SEND_MESSAGE( object, message->peer_id, va("Закодированная строка: %s", base64_encoded_string ), NULL, 0 );
      free(base64_encoded_string);
    }
}

int base64_string(const char *base64_str);

void cmd_base64_decode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Использование: b64d <строка>", NULL, 0 );
      return;
    }

  if( !base64_string(args) )
    {
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Строка имеет недопустимые символы для base64", NULL, 0 );
      return;
    }

  char *base64_decoded_string = (char*)base64_decode( (const unsigned char *)args, strlen( args ), NULL );
  if(base64_decoded_string)
    {
      VKAPI_SEND_MESSAGE( object, message->peer_id, va( "Декодированная строка: %s", base64_decoded_string ), NULL, 0 );
      free(base64_decoded_string);
    }
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
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Использование: ранд максимальное число\nранд минимальное число максимальное число\n", NULL, 0 );
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
      VKAPI_SEND_MESSAGE( object, message->peer_id, va("Случайное число: %lli", random_int64( atoll( argv[1] ), atoll( argv[2] ) ) ), NULL, 0 );
    }
  else {
      VKAPI_SEND_MESSAGE( object, message->peer_id, va("Случайное число: %lli", random_int64( 1, atoll( argv[1] ) ) ), NULL, 0 );
    }

}

void cmd_rand_date(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  unsigned int seed = UINT_MAX;
  string_t s = STRING_INIT();
  time_t date = 0;

  if(argv[1])
    {
    seed = CRC32( (const unsigned char *)args, strlen(args) );
    date = random_time( time(NULL), seed );
    } else {
     date = random_time( time(NULL), seed );
    }

  strftime(s->ptr, s->size, "Это произойдёт %e %B %Y", localtime( &date ));

  VKAPI_SEND_MESSAGE( object, message->peer_id, s->ptr, NULL, 0 );
  STRING_DESTROY(s);
}

void cmd_who(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
//    string_t method_args = STRING_INIT();
//    STRING_FORMAT(method_args, "peer_id=%i", message->peer_id);
//    string_t s = VKAPI_CALL_METHOD(object, "messages.getConversationMembers", method_args, true);
//    VKAPI_SEND_MESSAGE(object, message->peer_id, s->ptr);
}

void cmd_flip(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  int64_t random = random_int64( 0, 10000 );

  if( random >= 5000 )
    {
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Орёл!", NULL, 0 );
    } else {
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Решка!", NULL, 0 );
   }
}

void cmd_info(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Использование: инфа <строка>", NULL, 0 );
      return;
    }

  int64_t percents = random_int64( 0, 172 );

  VKAPI_SEND_MESSAGE( object, message->peer_id, va("Вероятность этого составляет %lli%%", percents), NULL, 0 );
}

void cmd_rate(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        VKAPI_SEND_MESSAGE( object, message->peer_id, "Использование: оцени <строка>", NULL, 0 );
        return;
      }

    int64_t rate = random_int64( 0, 11 );

    VKAPI_SEND_MESSAGE( object, message->peer_id, va("Я оцениваяю %s в %lli/10", args, rate), NULL, 0 );
}

int cmds_choose_tokeinize_cmd( char *str, char *tokens[], int *tokens_len );

void cmd_choose(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if(!args || argc < 3)
    {
        VKAPI_SEND_MESSAGE(object, message->peer_id, "Недостаточно аргументов\n Импользование: <строка> или <строка> ", NULL, 0);
        return;
    }

    char **tokens = calloc(256, sizeof(char*));

    int tokens_len = 0;

    string_t s = STRING_INIT();

    STRING_COPY(s, args);

    cmds_choose_tokeinize_cmd( s->ptr, tokens, &tokens_len );

    int64_t rand = random_int64(0, tokens_len);

    VKAPI_SEND_MESSAGE(object, message->peer_id, va("Я выбираю %s", tokens[rand]), NULL, 0);
    STRING_DESTROY(s);
    free(tokens);
}

void cmd_rand_docs(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        VKAPI_SEND_MESSAGE( object, message->peer_id, "Использование: доки <строка для поиска>", NULL, 0 );
        return;
      }

    string_t s = STRING_INIT();

    STRING_FORMAT(s, "q=%s&count=10", args);

    string_t result = VKAPI_CALL_METHOD( object, "docs.search", s, true);

    cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

    cJSON *resp = cJSON_GetObjectItem(ptr, "response");

    printf("HMM: %s\n", result->ptr);

    cJSON *count = cJSON_GetObjectItem(resp, "count");

    if(!count->valueint)
    {
       cJSON_Delete(ptr);

       VKAPI_SEND_MESSAGE(object, message->peer_id, "По вашему запросу нету документов", NULL, 0);

       STRING_DESTROY(s);
       STRING_DESTROY(result);
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

    VKAPI_SEND_MESSAGE(object, message->peer_id, "Документы по вашему запросу:", attaches, docs_count);

    cJSON_Delete(ptr);

    STRING_DESTROY(s);
    STRING_DESTROY(result);
}

void cmd_valute_curse(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args )
{
    if(argc > 2 || argc < 1)
    {
        VKAPI_SEND_MESSAGE(object, message->peer_id, "Для того чтобы узнать какие курсы валют доступны используйте курс помощь", NULL, 0);
        return;
    }

    if(!strcasecmp("помощь", argv[1]))
    {
        string_t s = STRING_INIT();
        string_t s2 = STRING_INIT();

        STRING_FORMAT(s2, "https://www.cbr-xml-daily.ru/daily_json.js" );

        if(!CURL_GET(object->curl_handle, s2, NULL, s ))
        {
            STRING_DESTROY(s);
            STRING_DESTROY(s2);

            VKAPI_SEND_MESSAGE(object, message->peer_id, "Oops", NULL, 0);
            return;
        }

        STRING_DESTROY(s2);

        s2 = STRING_INIT();

        char *info = va( "• Код - Название валюты\n" );

        STRING_STRNCAT( s2, info, strlen(info) );

        cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false), *valute_element = NULL, *array_value = NULL;

        array_value = cJSON_GetObjectItem(ptr, "Valute");

        cJSON_ArrayForEach( valute_element, array_value )
        {

            cJSON *CharCode = cJSON_GetObjectItem(valute_element, "CharCode");
            cJSON *Name = cJSON_GetObjectItem(valute_element, "Name");

            info = va("• %s - %s\n", cJSON_GetStringValue(CharCode), cJSON_GetStringValue(Name));

            STRING_STRNCAT(s2, info, strlen(info));
        }

        VKAPI_SEND_MESSAGE(object, message->peer_id, s2->ptr, NULL, 0);

        cJSON_Delete(ptr);
        STRING_DESTROY(s);
        STRING_DESTROY(s2);
        return;
    }

    string_t s = STRING_INIT();
    string_t s2 = STRING_INIT();

    STRING_FORMAT(s2, "https://www.cbr-xml-daily.ru/daily_json.js" );

    if(!CURL_GET(object->curl_handle, s2, NULL, s ))
    {
        STRING_DESTROY(s);
        STRING_DESTROY(s2);

        VKAPI_SEND_MESSAGE(object, message->peer_id, "Oops", NULL, 0);
        return;
    }

    STRING_DESTROY(s2);

    s2 = STRING_INIT();

    cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false), *valute = NULL, *array_value = NULL;

    array_value = cJSON_GetObjectItem(ptr, "Valute");

    valute = cJSON_GetObjectItem(array_value, argv[1]);

    if(!valute)
    {
        VKAPI_SEND_MESSAGE(object, message->peer_id, "Валюты с таким кодом нету!", NULL, 0);

        cJSON_Delete(ptr);
        STRING_DESTROY(s);
        STRING_DESTROY(s2);
        return;
    }

    cJSON *Value = cJSON_GetObjectItem(valute, "Value");
    cJSON *Previous = cJSON_GetObjectItem(valute, "Previous");
    cJSON *Name = cJSON_GetObjectItem(valute, "Name");

    double diff = Value->valuedouble - Previous->valuedouble;

    char *info = NULL;

    if(diff < 0.0)
    {
    info = va("Курс валют\n%s: %f ₽\n↓ %f ₽\n", cJSON_GetStringValue(Name), Value->valuedouble, diff );
    } else {
    info = va("Курс валют\n%s: %f ₽\n↑ %f ₽\n", cJSON_GetStringValue(Name), Value->valuedouble, diff );
    }

    STRING_STRNCAT(s2, info, strlen(info));

    VKAPI_SEND_MESSAGE(object, message->peer_id, s2->ptr, NULL, 0);

    cJSON_Delete(ptr);
    STRING_DESTROY(s);
    STRING_DESTROY(s2);
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
      VKAPI_SEND_MESSAGE( object, message->peer_id, "Использование: погода <Город>", NULL, 0 );
      return;
    }

  string_t url_get = STRING_INIT();
  string_t openweather_json = STRING_INIT();

  STRING_FORMAT(url_get, "%s/%s?q=%s&appid=%s&lang=%s&units=metric", OPEN_WEATHER_URL, OPEN_WEATHER_METHOD, argv[1], OPEN_WEATHER_TOKEN, OPEN_WEATHER_LANG );

  vkapi_boolean error_code = CURL_GET(object->curl_handle, url_get, NULL, openweather_json );

  STRING_DESTROY(url_get);

  if(!error_code)
    {
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Произошла неизвестная ошибка", NULL, 0);
      STRING_DESTROY(openweather_json);
      return;
    }

    if(argv[argc])
      {
    if(!strncasecmp(argv[argc], "json", 4))
      {
    VKAPI_SEND_MESSAGE(object, message->peer_id, openweather_json->ptr, NULL, 0);
    STRING_DESTROY(openweather_json);
	return;
      }
      }

  cJSON *ptr = cJSON_ParseWithOpts(openweather_json->ptr, NULL, false);

  if(!ptr)
    {
    VKAPI_SEND_MESSAGE(object, message->peer_id, "cJSON сделал рыг рыг пук пук", NULL, 0);
    STRING_DESTROY(openweather_json);
	return;
    }

  cJSON *cod = cJSON_GetObjectItem(ptr, "cod");


        if(cod->valueint == 200)
        {
      string_t msg = STRING_INIT();

	  cJSON *weather_array = cJSON_GetObjectItem(ptr, "weather");
	  cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
	  cJSON *name = cJSON_GetObjectItem(ptr, "name");
	  cJSON *main_obj = cJSON_GetObjectItem(ptr, "main");
	  cJSON *temp = cJSON_GetObjectItem(main_obj, "temp");

      STRING_FORMAT(msg, "Погода в %s\n\n• Сейчас: %i℃, %s\n", cJSON_GetStringValue(name), temp->valueint, cJSON_GetStringValue(cJSON_GetObjectItem(weather, "description")));

      VKAPI_SEND_MESSAGE(object, message->peer_id, msg->ptr, NULL, 0);

      STRING_DESTROY(msg);
	  cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
	  return;
        } else if(!strncmp(cod->valuestring, "404", 3))
	{
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Такого города нет", NULL, 0);
	  cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
	  return;
	} else if(!strncmp(cod->valuestring, "401", 3)) {
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Ограничение апи openweathermap", NULL, 0);
	  cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
	  return;
	} else {
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Неизвестная ошибка", NULL, 0);
	  cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
	  return;
	}
}

void cmd_crc32(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
   string_t msg = STRING_INIT();
   string_t file = STRING_INIT();

   if(argc < 2 && !message->attachmens)
    {
      usage:
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Использование: crc32 <строка> или прикреплённый документ", NULL, 0);
      STRING_DESTROY(msg);
      STRING_DESTROY(file);
      return;
    }

  uint32_t crc32 = CRC32((const unsigned char *)args, strlen(args));

  if(argc > 1)
  VKAPI_SEND_MESSAGE(object, message->peer_id, va("CRC32 хеш строки: 0x%X", crc32), NULL, 0);
  else
    {
      cJSON *attach = NULL;

      cJSON_ArrayForEach(attach, (cJSON*)message->attachmens)
      {

      cJSON *doc = cJSON_GetObjectItem(attach, "doc");

      if(!doc)
	{
    STRING_DESTROY(file);
	goto usage;
	}
      string_t url = STRING_INIT();

      STRING_COPY(url, cJSON_GetStringValue(cJSON_GetObjectItem(doc, "url")));

      CURL_GET(object->curl_handle, url, NULL, file);

      char *ptr = va("CRC32 хеш файла %s: 0x%X\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), CRC32((const unsigned char *)file->ptr, file->len));

      STRING_STRNCAT(msg, ptr, strlen(ptr));

      STRING_DESTROY(file);
      }

      VKAPI_SEND_MESSAGE(object, message->peer_id, msg->ptr, NULL, 0);
      STRING_DESTROY(msg);
    }
}

void cmd_hlmemes(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  VKAPI_SEND_MESSAGE(object, message->peer_id, "Держи: https://www.youtube.com/playlist?list=PLxdH4XPU7OZnR5XV-b1QmnM7yCwAlmOUd", NULL, 0);
}

void cmd_cat(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = STRING_INIT();
  string_t url = STRING_INIT();

  STRING_COPY(url, "https://aws.random.cat/meow");

  if(CURL_GET(object->curl_handle, url, NULL, s) != true)
    {
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      STRING_DESTROY(s);
      STRING_DESTROY(url);
      return;
    }

  cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false);

  if(!ptr)
    {
      VKAPI_SEND_MESSAGE(object, message->peer_id, "Увы но сервер с котиками недоступен >:(", NULL, 0);
      STRING_DESTROY(s);
      STRING_DESTROY(url);
      return;
    }

  cJSON *file = cJSON_GetObjectItem(ptr, "file");

  STRING_COPY(url, cJSON_GetStringValue(file));

  string_t filedata = STRING_INIT();

  CURL_GET(object->curl_handle, url, NULL, filedata);

  vkapi_attach *attach = VKAPI_UPLOAD_DOC(object, message, "random_cat.png", filedata, VKAPI_PHOTO);

  VKAPI_SEND_MESSAGE(object, message->peer_id, NULL, attach, 1);

  cJSON_Delete(ptr);

  STRING_DESTROY(s);
  STRING_DESTROY(filedata);
  STRING_DESTROY(url);
}

//TODO: make get_privilgae_by_id in engine api
//void cmd_get_privilage(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
//{
//    privilage_t priv = get_privilage_by_id(message->from_id);
//    string_t s = STRING_INIT();
//    STRING_FORMAT(s, "Ваш уровень привелегий - %s\n", get_name_privilage(priv));
//    VKAPI_SEND_MESSAGE(object, message->peer_id, s->ptr, NULL, 0);
//    STRING_DESTROY(s);
//    return;
//}

void cmd_debug(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if(argv[1])
    {
      if(argv[2])
	{
      string_t s = STRING_INIT();

	      for( int c = 2; c < argc + 1; c++ )
		{
		  if( !argv[c] )
		    break;

		  if(c > 2)
            STRING_STRNCAT( s, " ", 1 );

          STRING_STRNCAT( s, argv[c], strlen(argv[c]) );
		}

      MEMCACHE_PUSH(argv[1], s->ptr);
      VKAPI_SEND_MESSAGE(object, message->peer_id, va("Memcache: push key \"%s\" value \"%s\"\n", argv[1], s->ptr), NULL, 0);
	  return;
	}
      const char *value = MEMCACHE_GET(argv[1]);
      if(value)
      VKAPI_SEND_MESSAGE(object, message->peer_id, va("Memcache: key is \"%s\" value is \"%s\"\n", argv[1], value), NULL, 0);
      else
    VKAPI_SEND_MESSAGE(object, message->peer_id, va("Memcache: value for key \"%s\" not found\n", argv[1]), NULL, 0);
    }
}
