#include <enginecallbacks.h>
#include <limits.h>
#include <time.h>
#include "va_utils.h"

#include "minijson.h"

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

void cmd_rand_docs(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if( argc < 1 )
      {
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: доки <строка для поиска>", NULL, 0 );
        return;
      }

    curl_postfield_t post = CURL_POSTFIELD_INIT();
    CURL_POSTFIELD_PUSH(post, "q", args);
    CURL_POSTFIELD_PUSH(post, "count", "10");
    string_t result = VKAPI_CALL_METHOD( "docs.search", post, true);
    CURL_POSTFIELD_DESTROY(post);

	minijson *ptr = minijson_parse(result->ptr);

	minijson *resp = minijson_getobjectitem(ptr, "response");

	minijson *count = minijson_getobjectitem(resp, "count");

    if(!count->valueint)
    {
	   minijson_delete(ptr);

       VKAPI_SEND_MESSAGE( message->peer_id, "По вашему запросу нету документов", NULL, 0);
       STRING_DESTROY(result);
       return;
    }

	minijson *items = minijson_getobjectitem(resp, "items");

	int docs_count = minijson_getarraysize(items);

    vkapi_attach *attaches = (vkapi_attach*)calloc(docs_count, sizeof(vkapi_attach));

    for(int i = 0; i < docs_count; i++)
    {
		minijson *obj = minijson_getarrayitem(items, i);

        attaches[i].type = VKAPI_DOC;
		attaches[i].media_id = minijson_getobjectitem(obj, "id")->valueint;
		attaches[i].owner_id = minijson_getobjectitem(obj, "owner_id")->valueint;
    }

    VKAPI_SEND_MESSAGE( message->peer_id, "Документы по вашему запросу:", attaches, docs_count);

	minijson_delete(ptr);

    STRING_DESTROY(result);
}
