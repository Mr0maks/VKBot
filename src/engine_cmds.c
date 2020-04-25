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

  string_format( s, "VKBot\nБот написан на C\nИспользует библиотеки libcurl и cjson\nСобран %s %s\nВерсия %s", __DATE__, __TIME__, VERSION );

  vkapi_send_message(message->peer_id, s->ptr, NULL, 0);

  string_destroy(s);
}

void cmd_stat(vkapi_message_object *message, int argc, char **argv, const char *args)
{
//  string_t s = string_init();
//  string_format( s, "Статистика бота\nКоличество работающих воркеров: %i\nПик очереди: %lu\nПамяти сожрано мной: %ld кб\nМаксимальный размер кеша: %lu кб\nСъел кеш: %lu кб\nКомманд обработано: %lu\nСообщений обработано: %lu\n", worker_get_workers_count(), queue_maxium_tasks(), GC_get_free_bytes() / 1000,memcache_get_max_size(), memcache_get_size() / 1024, worker_commands_processed(), worker_message_processed() );
//  vkapi_send_message( message->peer_id, s->ptr, NULL, 0 );
//  string_destroy( s );
}
