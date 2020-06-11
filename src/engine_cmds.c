#include "common.h"

extern cmds_t commands[];
extern cmds_modules_pools_t *modules_cmds_poll;
extern module_t *modules_pool;

void cmd_help(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();

  for(int i = 0; commands[i].string != NULL; i++ )
    {
      if(commands[i].description)
      {
      char *str = va( "• %s - %s\n", commands[i].string, commands[i].description );
      string_strncat( s, str, strlen( str ) );
      }
    }

  cmds_modules_pools_t *ptr = modules_cmds_poll;

  while (ptr != NULL) {
      if(ptr->description)
      {
      char *str = va( "• %s - %s\n", ptr->string, ptr->description );
      string_strncat( s, str, strlen( str ) );
      }
      ptr = ptr->next;
    }

  vkapi_send_message( message->peer_id, s->ptr, NULL, 0 );
  string_destroy( s );
}

void cmd_modules(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    string_t s = string_init();

    module_t *ptr = modules_pool;

    char *str = NULL;

    str = "Имя с которым загружен - Автор Версия Дата сборки Ссылка\n";

    string_strncat(s, str, strlen(str) );

    while (ptr != NULL) {
        str = va( "• %s - %s %s %s %s\n", ptr->name, ptr->info->author, ptr->info->version, ptr->info->date, ptr->info->url );
        string_strncat( s, str, strlen( str ) );
        ptr = ptr->next;
      }

    vkapi_send_message( message->peer_id, s->ptr, NULL, 0 );
    string_destroy( s );
}

void cmd_about_bot(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_format( s, "VKBot\nИспользует библиотеки libcurl и cJSON\nВерсия API ВК: %s\nСобран %s %s\nВерсия %s\nАрхитектура CPU %s", VK_API_VERSION, __DATE__, __TIME__, VERSION, ARCH );
  vkapi_send_message(message->peer_id, s->ptr, NULL, 0);
  string_destroy(s);
}

void cmd_stat(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_format( s, "Статистика бота\nКоличество работающих воркеров: %i\nКомманд обработано: %lu\nСообщений обработано: %lu\n", worker_get_workers_count(), worker_commands_processed(), worker_message_processed() );
  vkapi_send_message( message->peer_id, s->ptr, NULL, 0 );
  string_destroy( s );
}
