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

cmds_hashs_t *cached_cmds = NULL;

cmds_name_hashs_t *cached_names = NULL;

const cmds_t commands[] = {
  { "помощь", "команда для показа этого сообщения", cmd_help },
  { "ping", "команда для проверки бота на отзывчевость", cmd_ping },
  { "b64e", "кодирует строку в формате base64", cmd_base64_encode },
  { "b64d", "декодирует строку в формате base64", cmd_base64_decode },
  { "стат", "показывает разную статистику бота", cmd_stat },
  { "ранд", "рандомное число", cmd_rand },
  { "когда", "узнать дату события", cmd_rand_date },
  { "инфа", "узнать вероятность чего-либо", cmd_info },
  { "флип", "подбросить монетку", cmd_flip },
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

cmds_hashs_t *cmd_call_command_if_exits(const char *command)
{
  if(!command)
    return NULL;

  size_t command_len = strlen( command );

  if( command_len > max_command_len )
    return NULL;

  unsigned int name_hash = crc32_calc( (const unsigned char *)command, command_len );

  for( size_t i = 0; i < static_commands; i++ ) {
      if( cached_cmds[i].hash == name_hash )
	{
	  return &cached_cmds[i];
	}
    }

  return NULL;
}

vkapi_bool cmd_handle(vkapi_object *object, vkapi_message_new_object *message)
{
  char *saveptr = NULL;
  char *argv[64] = { NULL };
  char *token = NULL;
  string_t s = string_dublicate( message->text );
  string_t args_s = string_init();

  //TODO: Refactor this shit

  if( message->text->len == 0 || !message->text->ptr )
    {
    goto end;
    }

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

   cmds_hashs_t *cmd = cmd_call_command_if_exits(argv[0]);

   if(cmd)
     {
       if(cmd->function)
	 cmd->function(object, message, i - 1, argv, args_s->ptr);
       goto end_true;
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

end_true:
    string_destroy( s );
    string_destroy( args_s );
    return true;

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

void cmd_calculate_hashes()
{
  cmd_calculate_cmd_hashes();
  cmd_calculate_name_hashes();
}

void cmd_handler_init()
{
  cmd_calculate_hashes();
}
