#include "common.h"
#include <sys/resource.h>

extern cmds_t commands[];
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

    module_t *ptr = modules_pool;
    while(ptr != NULL) {

        cmd_module_pool_t *ptr2 = ptr->cmd_pool;
        while (ptr2 != NULL) {
            if(ptr2->description)
            {
                char *str = va( "• %s - %s\n", ptr2->string, ptr2->description );
                string_strncat( s, str, strlen( str ) );
            }
            ptr2 = ptr2->next;
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
        str = va( "• %s - %s %s %s %s\n", ptr->name, ptr->info.author, ptr->info.version, ptr->info.date, ptr->info.url );
        string_strncat( s, str, strlen( str ) );
        ptr = ptr->next;
      }

    vkapi_send_message( message->peer_id, s->ptr, NULL, 0 );
    string_destroy( s );
}

void cmd_about_bot(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();
  string_format( s, "VKBot\nИспользует библиотеки libcurl и cJSON\nИсходный код: https://github.com/Mr0maks/VKBot\nВерсия API ВК: %s\nВерсия API для модулей: %i\nСобран: %s %s\nВерсия: %s\nАрхитектура CPU: %s", VK_API_VERSION, ENGINE_API_VERSION, __DATE__, __TIME__, VERSION, ARCH );
  vkapi_send_message(message->peer_id, s->ptr, NULL, 0);
  string_destroy(s);
}

time_t Host_Init_Time(void);

void cmd_stat(vkapi_message_object *message, int argc, char **argv, const char *args)
{
  string_t s = string_init();

  struct rusage r_usage;
  getrusage(RUSAGE_SELF,&r_usage);

  time_t date = Host_Init_Time();

  char buffer[256];
  strftime(buffer, sizeof (buffer), "%c", localtime( &date ));

#ifdef VKBOT_FIND_LEAK
    size_t memory_use = GC_get_heap_size() / 1000;
    vkapi_send_message( message->peer_id, va("-DVKBOT_FIND_LEAK\nGC_get_heap_size: %zu kb\n", memory_use), NULL, 0 );
#endif

  string_format( s, "Статистика бота\nБот запущен: %s\nКоличество работающих воркеров: %i\nКомманд обработано: %lu\nПамяти использовано: %ld кб\nСообщений обработано: %lu\n", buffer, worker_get_workers_count(), worker_commands_processed(), r_usage.ru_maxrss, worker_message_processed() );
  vkapi_send_message( message->peer_id, s->ptr, NULL, 0 );
  string_destroy( s );
}
