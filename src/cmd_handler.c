#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <pthread.h>

#include "cmd_handler.h"
#include "cmds.h"
#include "crc32_hash.h"
#include "vkapi.h"
#include "va_utils.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define ARRAY_LENGHT(x) (sizeof(x)/sizeof(x[0])) - 1

cmds_modules_pools_t *modules_cmds_poll = NULL;

cmds_hashs_t *cached_cmds = NULL;

cmds_name_hashs_t *cached_names = NULL;

const cmds_t commands[] = {
  { "помощь", "команда для показа этого сообщения", cmd_help },
  { "оботе", "о боте", cmd_about_bot },
  { "ping", "команда для проверки бота на отзывчевость", cmd_ping },
  { "b64e", "кодирует строку в формате base64", cmd_base64_encode },
  { "b64d", "декодирует строку в формате base64", cmd_base64_decode },
  { "стат", "показывает разную статистику бота", cmd_stat },
  { "ранд", "рандомное число", cmd_rand },
  { "когда", "узнать дату события", cmd_rand_date },
//  { "кто", "выберает рандомного человека из беседы (нужны права администратора)", cmd_who },
  { "инфа", "узнать вероятность чего-либо", cmd_info },
  { "флип", "подбросить монетку", cmd_flip },
  { "погода", "показывает погоду сейчас", cmd_weather },
  { "crc32", "подсчитывает crc32 хеш строки или файла", cmd_crc32 },
#ifdef DEBUG
  { "debug", "бот собран с отладочными функциями", cmd_debug },
#endif
  { NULL, NULL, NULL }
};

const cmds_names_t names[] = {
  { "Максбот" },
  { "максбот" },
  { "Максимбот" },
  { "максимбот" },
  { "Максимушка" },
  { "максимушка" },
  { "/" },
  { NULL }
};

static size_t static_names = ARRAY_LENGHT( names );
static size_t static_commands = ARRAY_LENGHT( commands );

static size_t max_command_len = 0;
static size_t max_name_len = 0;

vkapi_bool cmd_is_bot_name(const char *name)
{
  if(!name)
    return false;

  size_t name_len = strlen( name );

  if( name_len > max_name_len )
    return false;

  unsigned int name_hash = crc32_calc( (const unsigned char *)name, name_len );

  for( size_t i = 0; i < static_names; i++ ) {
      if( cached_names[i].hash == name_hash )
	{
  	return true;
	}
    }

  return false;
}

cmd_function_callback cmd_get_command(const char *command)
{
  if(!command)
    return NULL;

  size_t command_len = strlen( command );

  if( command_len > max_command_len )
    return NULL;

  unsigned int cmd_hash = crc32_calc( (const unsigned char *)command, command_len );

  for( size_t i = 0; i < static_commands; i++ ) {
      if( cached_cmds[i].hash == cmd_hash )
	{
	  return cached_cmds[i].function;
	}
    }

  cmds_modules_pools_t *ptr = modules_cmds_poll;

  while (ptr) {

      if(ptr->hash == cmd_hash)
	{
	  return ptr->function;
	}
      ptr = ptr->next;
    }

  return NULL;
}

vkapi_bool cmd_handle(vkapi_handle *object, vkapi_message_object *message)
{
  char *saveptr = NULL;
  char *argv[64] = { NULL };
  char *token = NULL;

  if( message->text->len == 0 || !message->text->ptr )
    {
      return false;
    }

  string_t s = string_dublicate( message->text );
  string_t args_s = string_init();

   token = strtok_r( s->ptr, " ", &saveptr );

   if( !token )
     goto end;

   if( !cmd_is_bot_name(token) )
    {
      goto end;
    }

   int i = 0;
   while ( token != NULL ) {
       token = strtok_r( NULL, " ", &saveptr );
        if( token )
	    argv[i++] = token;
     }

   if( !argv[0] )
     {
     goto dada;
     } else {
       for( int c = 1; c < i; c++ )
	 {
	  if( !argv[c] )
	     break;

	   if(c > 1)
	       string_strncat( args_s, " ", 1 );

	   string_strncat( args_s, argv[c], strlen(argv[c]) );
	 }
     }

   printf( "Try to call cmd %s\n", argv[0] );

   cmd_function_callback cmd = cmd_get_command(argv[0]);

   if(cmd)
     {
       cmd(object, message, i - 1, argv, args_s->ptr);

       string_destroy( s );
       string_destroy( args_s );
       return true;
     } else
       goto not_found;

dada:
    vkapi_send_message( object, message->peer_id, "Да-да?\n Для того чтобы узнать команды используйте помощь." );
    string_destroy( s );
    string_destroy( args_s );
    return false;

not_found:
    vkapi_send_message( object, message->peer_id, "Команда не найдена\n Для того чтобы узнать команды используйте помощь." );
    string_destroy( s );
    string_destroy( args_s );
    return false;

end:
    string_destroy( s );
    string_destroy( args_s );
    return false;
}

void cmd_calculate_cmd_hashes()
{
  cached_cmds = (cmds_hashs_t *)calloc( static_commands, sizeof(cmds_hashs_t) );

  if( cached_cmds == NULL )
    {
      printf( "Error while allocated memory for commands hashs\n" );
      exit( EXIT_FAILURE );
    }

  printf( "Static commands in bot %lu ( static cmds_t commands[] )\n", static_commands );

  for( size_t i = 0; i < static_commands; i++ )
    {
      if( commands[i].string == NULL )
	break;

      size_t string_len = strlen( commands[i].string );

      cached_cmds[i].hash = crc32_calc( (const unsigned char *)commands[i].string, string_len );
      cached_cmds[i].function = commands[i].function;

      max_command_len = MAX( max_command_len, string_len );

      printf( "\"%s\" \"%s\" hash: %X len: %lu\n", commands[i].string, commands[i].description, cached_cmds[i].hash, string_len );

    }
}

void cmd_calculate_name_hashes()
{
  cached_names = (cmds_name_hashs_t *)calloc( static_names, sizeof(cmds_name_hashs_t) );

  if( cached_names == NULL )
    {
      printf( "Error while allocated memory for commands hashs\n" );
      exit( EXIT_FAILURE );
    }

  printf( "Static names in bot %lu ( static cmds_names_t names[] )\n", static_names );

  for( size_t i = 0; i < static_names; i++ )
    {
      if( names[i].name == NULL )
	break;

      size_t string_len = strlen( names[i].name );

      cached_names[i].hash = crc32_calc( (const unsigned char *)names[i].name, string_len );

      max_name_len = MAX( max_name_len, string_len );

      printf( "\"%s\" hash: %X\n", names[i].name, cached_names[i].hash );
    }
}

void cmd_handler_register_module_cmd(int module_id, const char *cmd_name, const char *description, cmd_function_callback callback)
{
  cmds_modules_pools_t *ptr = NULL;

  if(cmd_name == NULL || description == NULL || callback == NULL)
    return;

  ptr = malloc(sizeof(cmds_modules_pools_t));

  if(!ptr)
    {
      return;
    }

  ptr->hash = crc32_calc((const unsigned char*)cmd_name, strlen(cmd_name));

  ptr->string = cmd_name;
  ptr->description = description;
  ptr->module_id = module_id;
  ptr->function = callback;

  ptr->next = modules_cmds_poll;
  modules_cmds_poll = ptr;

  printf("Cmd from module: \"%s\" - \"%s\" hash: %X \n", cmd_name, description, ptr->hash);

  max_command_len = MAX(max_command_len, strlen(cmd_name));

}

void cmd_handler_unregister_module_cmd(int module_id)
{
    cmds_modules_pools_t *ptr = modules_cmds_poll, *ptr_t1 = NULL;
    if(ptr->module_id == module_id)
      {
	modules_cmds_poll = ptr->next;
	return;
      }

    ptr_t1 = ptr->next;

    while(ptr_t1)
      {

	if( ptr_t1->module_id == module_id )
	  {
	    ptr->next = ptr_t1->next;
	    return;
	  }

	ptr = ptr_t1;
	ptr_t1 = ptr_t1->next;
      }
}

void cmd_calculate_hashes()
{
  cmd_calculate_cmd_hashes();
  cmd_calculate_name_hashes();
}

void cmd_handler_deinit()
{
  free(cached_cmds);
  free(cached_names);
}

void cmd_handler_init()
{
  cmd_calculate_hashes();
}
