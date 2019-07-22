#include "vkapi.h"
#include "va_utils.h"
#include <string.h>
#include <time.h>
#include <limits.h>
#include <crc32_hash.h>
#include "base64_decoder.h"

#include "worker_queue.h"

#include "cmd_handler.h"

extern cmds_t commands[];

void cmd_help(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();

  for(int i = 0; commands[i].string != NULL; i++ )
    {
      char *str = va( "• %s - %s\n", commands[i].string, commands[i].description );
      string_strncat( s, str, strlen( str ) );
    }
  vkapi_send_message( object, message->peer_id, s->ptr );
  string_destroy( s );
}

void cmd_ping(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message( object, message->peer_id, "Pong" );
}

void cmd_base64_encode(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  string_t s = NULL;

  if( argc < 1 )
    {
      vkapi_send_message(object, message->peer_id, "Использование: b64e <строка>");
      return;
    }

  s = string_init();
  base64_encode( (const unsigned char *)args, (unsigned char *)s->ptr, strlen( args ), 1);
  vkapi_send_message( object, message->peer_id, va("Закодированная строка: %s", s->ptr ) );
  string_destroy( s );
}

int base64_string(const char *base64_str);

void cmd_base64_decode(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
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

long get_param_of_path(const char *filenpath, const char *what);

void cmd_stat(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_format( s, "Статистика бота\nКоличество работающих воркеров: %i\nПик очереди: %lu\nПамяти сожрано мной: %ld кб\nВерсия: %s\n", worker_get_workers_count(), queue_maxium_tasks_in_queue(), get_param_of_path( "/proc/self/status", "VmRSS" ), VERSION );
  vkapi_send_message( object, message->peer_id, s->ptr );
  string_destroy( s );
}

int64_t random_int64(int64_t min, int64_t max)
{
  return min + rand() % ( max + 1 - min);
}

time_t random_time(time_t min, time_t max)
{
  return min + rand() % ( max + 1 - min );
}

void cmd_rand(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
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

void cmd_rand_date(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
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

void cmd_flip(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  int64_t random = random_int64( 0, 10000 );

  if( random >= 5000 )
    {
      vkapi_send_message( object, message->peer_id, "Орёл!" );
    } else {
      vkapi_send_message( object, message->peer_id, "Решка!" );
   }
}

void cmd_info(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  if( argc < 1 )
    {
      vkapi_send_message( object, message->peer_id, "Использование: инфа <строка>" );
    }

  int64_t percents = random_int64( 0, 197 );

  vkapi_send_message( object, message->peer_id, va("Вероятность этого составляет %lli%%", percents) );
}

void cmd_debug(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
}
