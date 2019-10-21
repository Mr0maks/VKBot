#include <enginecallbacks.h>
#include <limits.h>
#include <time.h>
#include <gc_memmgr.h>
#include "va_utils.h"

#include <cJSON.h>

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

void cmd_rand(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 || argc > 2 )
    {
      usage:
      VKAPI_SEND_MESSAGE( message->peer_id, "Использование: ранд максимальное число\nранд минимальное число максимальное число\n", NULL, 0 );
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
      VKAPI_SEND_MESSAGE( message->peer_id, va("Случайное число: %lli", random_int64( atoll( argv[1] ), atoll( argv[2] ) ) ), NULL, 0 );
    }
  else {
      VKAPI_SEND_MESSAGE( message->peer_id, va("Случайное число: %lli", random_int64( 1, atoll( argv[1] ) ) ), NULL, 0 );
    }

}

void cmd_rand_date(vkapi_message_object *message, int argc, char **argv, const char *args)
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

  VKAPI_SEND_MESSAGE( message->peer_id, s->ptr, NULL, 0 );
  STRING_DESTROY(s);
}

void cmd_who(vkapi_message_object *message, int argc, char **argv, const char *args)
{
//    string_t method_args = STRING_INIT();
//    STRING_FORMAT(method_args, "peer_id=%i", message->peer_id);
//    string_t s = VKAPI_CALL_METHOD( "messages.getConversationMembers", method_args, true);
//    VKAPI_SEND_MESSAGE( message->peer_id, s->ptr);
}

void cmd_flip(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  int64_t random = random_int64( 0, 10000 );

  if( random >= 5000 )
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Орёл!", NULL, 0 );
    } else {
      VKAPI_SEND_MESSAGE( message->peer_id, "Решка!", NULL, 0 );
   }
}

void cmd_info(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      VKAPI_SEND_MESSAGE( message->peer_id, "Использование: инфа <строка>", NULL, 0 );
      return;
    }

  int64_t percents = random_int64( 0, 172 );

  VKAPI_SEND_MESSAGE( message->peer_id, va("Вероятность этого составляет %lli%%", percents), NULL, 0 );
}

void cmd_rate(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: оцени <строка>", NULL, 0 );
        return;
      }

    int64_t rate = random_int64( 0, 11 );

    VKAPI_SEND_MESSAGE( message->peer_id, va("Я оцениваяю %s в %lli/10", args, rate), NULL, 0 );
}

int cmds_choose_tokeinize_cmd( char *str, char *tokens[], int *tokens_len );

void cmd_choose(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if(!args || argc < 3)
    {
        VKAPI_SEND_MESSAGE( message->peer_id, "Недостаточно аргументов\n Импользование: <строка> или <строка> ", NULL, 0);
        return;
    }

    char **tokens = calloc(256, sizeof(char*));

    int tokens_len = 0;

    string_t s = STRING_INIT();

    STRING_COPY(s, args);

    cmds_choose_tokeinize_cmd( s->ptr, tokens, &tokens_len );

    int64_t rand = random_int64(0, tokens_len);

    VKAPI_SEND_MESSAGE( message->peer_id, va("Я выбираю %s", tokens[rand]), NULL, 0);
    STRING_DESTROY(s);
    free(tokens);
}

void cmd_rand_docs(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: доки <строка для поиска>", NULL, 0 );
        return;
      }

    string_t s = STRING_INIT();

    STRING_FORMAT(s, "q=%s&count=10", args);

    string_t result = VKAPI_CALL_METHOD( "docs.search", s, true);

    cJSON *ptr = cJSON_ParseWithOpts(result->ptr, NULL, false);

    cJSON *resp = cJSON_GetObjectItem(ptr, "response");

    //ALERT("HMM: %s\n", result->ptr);

    cJSON *count = cJSON_GetObjectItem(resp, "count");

    if(!count->valueint)
    {
       cJSON_Delete(ptr);

       VKAPI_SEND_MESSAGE( message->peer_id, "По вашему запросу нету документов", NULL, 0);

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

    VKAPI_SEND_MESSAGE( message->peer_id, "Документы по вашему запросу:", attaches, docs_count);

    cJSON_Delete(ptr);

    STRING_DESTROY(s);
    STRING_DESTROY(result);
}
