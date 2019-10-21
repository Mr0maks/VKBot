#include <enginecallbacks.h>
#include "va_utils.h"
#include <strings.h>
#include <time.h>
#include <string.h>

#include <cJSON.h>

extern time_t cmd_uptime_start;

void cmd_ping(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  VKAPI_SEND_MESSAGE( message->peer_id, "Pong", NULL, 0 );
}

void cmd_valute_curse(vkapi_message_object *message, int argc, char **argv, const char *args )
{
    if(argc > 2 || argc < 1)
    {
        VKAPI_SEND_MESSAGE( message->peer_id, "Для того чтобы узнать какие курсы валют доступны используйте курс помощь", NULL, 0);
        return;
    }

    if(!strcasecmp("помощь", argv[1]))
    {
        string_t s = STRING_INIT();
        string_t s2 = STRING_INIT();

        STRING_FORMAT(s2, "https://www.cbr-xml-daily.ru/daily_json.js" );

        if(!CURL_GET(NULL, s2, NULL, s ))
        {
            STRING_DESTROY(s);
            STRING_DESTROY(s2);

            VKAPI_SEND_MESSAGE( message->peer_id, "Oops", NULL, 0);
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

        VKAPI_SEND_MESSAGE( message->peer_id, s2->ptr, NULL, 0);

        cJSON_Delete(ptr);
        STRING_DESTROY(s);
        STRING_DESTROY(s2);
        return;
    }

    string_t s = STRING_INIT();
    string_t s2 = STRING_INIT();

    STRING_FORMAT(s2, "https://www.cbr-xml-daily.ru/daily_json.js" );

    if(!CURL_GET(NULL, s2, NULL, s ))
    {
        STRING_DESTROY(s);
        STRING_DESTROY(s2);

        VKAPI_SEND_MESSAGE( message->peer_id, "Oops", NULL, 0);
        return;
    }

    STRING_DESTROY(s2);

    s2 = STRING_INIT();

    cJSON *ptr = cJSON_ParseWithOpts(s->ptr, NULL, false), *valute = NULL, *array_value = NULL;

    array_value = cJSON_GetObjectItem(ptr, "Valute");

    valute = cJSON_GetObjectItem(array_value, argv[1]);

    if(!valute)
    {
        VKAPI_SEND_MESSAGE( message->peer_id, "Валюты с таким кодом нету!", NULL, 0);

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

    VKAPI_SEND_MESSAGE( message->peer_id, s2->ptr, NULL, 0);

    cJSON_Delete(ptr);
    STRING_DESTROY(s);
    STRING_DESTROY(s2);
}

#define OPEN_WEATHER_TOKEN "4a223e7db4e8c28265df2633448b69f3"
#define OPEN_WEATHER_URL "https://api.openweathermap.org"
#define OPEN_WEATHER_METHOD "data/2.5/weather"
#define OPEN_WEATHER_LANG "ru"

void cmd_weather(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Использование: погода <Город>", NULL, 0 );
      return;
    }

  string_t url_get = STRING_INIT();
  string_t openweather_json = STRING_INIT();

  STRING_FORMAT(url_get, "%s/%s?q=%s&appid=%s&lang=%s&units=metric", OPEN_WEATHER_URL, OPEN_WEATHER_METHOD, argv[1], OPEN_WEATHER_TOKEN, OPEN_WEATHER_LANG );

  bool error_code = CURL_GET(NULL, url_get, NULL, openweather_json );

  STRING_DESTROY(url_get);

  if(!error_code)
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Произошла неизвестная ошибка", NULL, 0);
      STRING_DESTROY(openweather_json);
      return;
    }

    if(argv[argc])
      {
    if(!strncasecmp(argv[argc], "json", 4))
      {
    VKAPI_SEND_MESSAGE( message->peer_id, openweather_json->ptr, NULL, 0);
    STRING_DESTROY(openweather_json);
    return;
      }
      }

  cJSON *ptr = cJSON_ParseWithOpts(openweather_json->ptr, NULL, false);

  if(!ptr)
    {
    VKAPI_SEND_MESSAGE( message->peer_id, "cJSON сделал рыг рыг пук пук", NULL, 0);
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

      VKAPI_SEND_MESSAGE( message->peer_id, msg->ptr, NULL, 0);

      STRING_DESTROY(msg);
      cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
      return;
        } else if(!strncmp(cod->valuestring, "404", 3))
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Такого города нет", NULL, 0);
      cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
      return;
    } else if(!strncmp(cod->valuestring, "401", 3)) {
      VKAPI_SEND_MESSAGE( message->peer_id, "Ограничение апи openweathermap", NULL, 0);
      cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
      return;
    } else {
      VKAPI_SEND_MESSAGE( message->peer_id, "Неизвестная ошибка", NULL, 0);
      cJSON_Delete(ptr);
      STRING_DESTROY(openweather_json);
      return;
    }
}

void cmd_crc32(vkapi_message_object *message, int argc, char **argv, const char *args)
{
   string_t msg = STRING_INIT();
   string_t file = STRING_INIT();

   if(!argc && !message->attachmens)
    {
      usage:
      VKAPI_SEND_MESSAGE( message->peer_id, "Использование: crc32 <строка> или прикреплённый документ", NULL, 0);
      STRING_DESTROY(msg);
      STRING_DESTROY(file);
      return;
    }

  uint32_t crc32 = CRC32((const unsigned char *)args, strlen(args));

  if(argc)
  VKAPI_SEND_MESSAGE( message->peer_id, va("CRC32 хеш строки: 0x%X", crc32), NULL, 0);
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

      CURL_GET(NULL, url, NULL, file);

      char *ptr = va("CRC32 хеш файла %s: 0x%X\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), CRC32((const unsigned char *)file->ptr, file->len));

      STRING_STRNCAT(msg, ptr, strlen(ptr));

      STRING_DESTROY(file);
      }

      VKAPI_SEND_MESSAGE( message->peer_id, msg->ptr, NULL, 0);
      STRING_DESTROY(msg);
    }
}
