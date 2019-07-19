#include "vkapi.h"
#include "va_utils.h"
#include <string.h>
#include "base64_decoder.h"

#include "worker_queue.h"

void cmd_help(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message(object, message->peer_id, "Обычно тут пишут помощь. Кто бы мне помог ААААААААААААААААААААААААААААААААА");
}

void cmd_ping(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message(object, message->peer_id, "Pong");
}

void cmd_base64_encode(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  string_t s = NULL;

  if(argc < 1)
    {
      vkapi_send_message(object, message->peer_id, "Использование: b64e <строка>");
      return;
    }

  s = string_init();
  base64_encode((const unsigned char *)args, (unsigned char *)s->ptr, strlen(args), 1);
  vkapi_send_message(object, message->peer_id, va("Закодированная строка: %s", s->ptr));
  destroy_string(s);
}

void cmd_base64_decode(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  string_t s = NULL;

  if(argc < 1)
    {
      vkapi_send_message(object, message->peer_id, "Использование: b64d <строка>");
      return;
    }

  s = string_init();
  base64_decode((const unsigned char *)args, (unsigned char *)s->ptr, strlen(args));
  vkapi_send_message(object, message->peer_id, va("Декодированная строка: %s", s->ptr));
  destroy_string(s);
}

int worker_get_workers_count();

void cmd_stat(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
  vkapi_send_message(object, message->peer_id, va("Статистика бота\nКоличество работающих воркеров: %i\nПик очереди: %lu\n", worker_get_workers_count()));
}

void cmd_debug(vkapi_object *object, vkapi_message_new_object *message, int argc, char **argv, const char *args)
{
}
