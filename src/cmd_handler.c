#include "common.h"
#include "cmd_handler.h"
#include "module.h"
#include "engine_cmds.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define ARRAY_LENGHT(x) (sizeof(x)/sizeof(x[0]))

typedef struct
{
    uint32_t hash;
    cmd_callback  function;
} cmd_hash_table_t;

typedef struct cmd_module_hash_table_s
{
    uint32_t hash;
    cmd_callback  function;
    struct cmd_module_hash_table_s *next;
} cmd_module_hash_table_t;

typedef struct
{
    const char *name;
} cmd_name_t;

static cmd_hash_table_t *cmd_hash_table = NULL;
static cmd_module_hash_table_t *cmd_modules_hash_table = NULL;
static uint32_t *name_hash_table = NULL;

const cmds_t commands[] = {
  { "помощь", "показывает это сообщение", cmd_help },
  { "модули", "список загруженных модулей", cmd_modules },
  { "оботе", "о боте", cmd_about_bot },
  { "стат", "показывает статистику бота", cmd_stat },
  { NULL, NULL, NULL }
};

const cmd_name_t names[] = {
  { "Максбот" },
  { "максбот" },
  { "Максимбот" },
  { "максимбот" },
  { "Максимушка" },
  { "максимушка" },
  { "/" },
  { NULL }
};

static size_t static_names = ARRAY_LENGHT( names ) - 1;
static size_t static_commands = ARRAY_LENGHT( commands ) - 1;

static size_t max_command_len = 0;
static size_t max_name_len = 0;

bool cmd_is_bot_name(const char *name)
{
  if(!name)
    return false;

  size_t name_len = strlen( name );

  if( name_len > max_name_len )
    return false;

  unsigned int name_hash = strncrc32(name, name_len );

  for( size_t i = 0; i < static_names; i++ ) {
      if( name_hash_table[i] == name_hash )
	{
  	return true;
	}
    }

  return false;
}

cmd_callback cmd_get_command(const char *command)
{
  if(!command)
    return NULL;

  size_t command_len = strlen( command );

  if( command_len > max_command_len )
    return NULL;

  uint32_t cmd_hash = strncrc32(command, command_len );

  for( size_t i = 0; i < static_commands; i++ ) {
      if( cmd_hash_table[i].hash == cmd_hash )
	{
      return cmd_hash_table[i].function;
    }
  }

  cmd_module_hash_table_t *ptr = cmd_modules_hash_table;

  while (ptr) {
      if(ptr->hash == cmd_hash)
	{
	  return ptr->function;
	}
      ptr = ptr->next;
    }

  return NULL;
}

bool cmd_handle(vkapi_message_object *message)
{
  char *argv[256] = { NULL };
  bool without_name = false;

  string_t s = string_dublicate( message->text );

  int tokens_count = 0;

  char **tokens = (char**)calloc(256, sizeof(char*));

  tokens_count = cmd_deserialize( (unsigned char*)s->ptr, tokens );

  string_t args_s = string_init();

   if( !cmd_is_bot_name(tokens[0]) )
    {
      if( message->private_message == true )
	{
      if( !cmd_get_command(tokens[0]) )
	    goto not_found;
	  else
	    without_name = true;
	} else
	goto end;
    }

   if(without_name == true)
     {
       if(!tokens[0])
           goto no_args;
     } else {
       if(!tokens[1])
           goto no_args;
     }

   if(without_name == true)
     {
       for(int i = 0; i < tokens_count; i++)
           argv[i] = tokens[i];
     } else {
       tokens_count--;
       for(int i = 0; i < tokens_count; i++)
           argv[i] = tokens[i + 1];
     }

       for(int c = 1; c < tokens_count; c++ )
	 {
	  if( !argv[c] )
	     break;

       else if (c > 1)
	        string_strncat( args_s, " ", 1 );

	   string_strncat( args_s, argv[c], strlen(argv[c]) );
	 }

   Con_Printf( "Try to call cmd %s\n", argv[0] );

   cmd_callback cmd = cmd_get_command(argv[0]);

   if(cmd)
     {
       cmd(message, tokens_count - 1, argv, args_s->ptr);

       string_destroy( s );
       string_destroy( args_s );
       free( tokens );
       return true;
     } else
       goto not_found;

no_args:
    vkapi_send_message( message->peer_id, "Да-да?\n Для того чтобы узнать команды используйте помощь.", NULL, 0 );
    goto end;

not_found:
    vkapi_send_message( message->peer_id, "Команда не найдена\n Для того чтобы узнать команды используйте помощь.", NULL, 0 );
end:
    string_destroy( s );
    string_destroy( args_s );

    if(tokens)
        free(tokens);

    return false;
}

static void cmd_build_cmd_hash_table()
{
  cmd_hash_table = (cmd_hash_table_t *)calloc( static_commands, sizeof(cmd_hash_table_t) );

  if( cmd_hash_table == NULL )
    {
      Con_Printf( "Error while allocated memory for commands hashs\n" );
      exit( EXIT_FAILURE );
    }

  Con_Printf( "Static commands in bot %lu ( static cmds_t commands[] )\n", static_commands );

  for( size_t i = 0; i < static_commands; i++ )
    {
      if( commands[i].string == NULL )
	break;

      size_t string_len = strlen( commands[i].string );

      cmd_hash_table[i].hash = strncrc32( commands[i].string, string_len );
      cmd_hash_table[i].function = commands[i].function;

      max_command_len = MAX( max_command_len, string_len );

      Con_Printf( "\"%s\" \"%s\"\n", commands[i].string, commands[i].description );

    }
}

static void cmd_build_name_hash_table()
{
  name_hash_table = (uint32_t *)calloc( static_names, sizeof(uint32_t) );

  if( name_hash_table == NULL )
    {
      Con_Printf( "Error while allocated memory for name hash table\n" );
      exit( EXIT_FAILURE );
    }

  for( size_t i = 0; i < static_names; i++ )
    {
      if( names[i].name == NULL )
	break;

      size_t string_len = strlen( names[i].name );

      name_hash_table[i] = strncrc32(names[i].name, string_len );

      max_name_len = MAX( max_name_len, string_len );

      Con_Printf( "\"%s\" hash: %X\n", names[i].name, name_hash_table[i]);
    }
}

void module_cmd(module_t *module, const char *cmd_name, const char *description, cmd_callback callback)
{
    assert(module);

  if(cmd_name == NULL || callback == NULL)
    return;

  cmd_module_pool_t *ptr = malloc(sizeof(cmd_module_pool_t));
  cmd_module_hash_table_t *ptr2 = malloc(sizeof (cmd_module_hash_table_t));

  if(!ptr || !ptr2)
    {
      if(ptr) free(ptr);
      if(ptr2) free(ptr2);
      return;
    }

  uint32_t hash = strncrc32(cmd_name, strlen(cmd_name));

  ptr->hash = hash;
  ptr2->hash = hash;

#ifdef VKBOT_HACKS
  //rust CString can't be static. Need alloc strings here
  ptr->string = strdup(cmd_name);

  if(description != NULL)
      ptr->description = strdup(description);
#else
  ptr->string = cmd_name;
  ptr->description = description;
#endif

  ptr->function = callback;
  ptr2->function = callback;

  ptr->next = module->cmd_pool;
  module->cmd_pool = ptr;

  ptr2->next = cmd_modules_hash_table;
  cmd_modules_hash_table = ptr2;

  Con_Printf("Command from module %s: \"%s\" - \"%s\"\n", module->name, cmd_name, description);

  max_command_len = MAX(max_command_len, strlen(cmd_name));
}

void cmd_handler_unregister_module_cmds(module_t *module)
{
}

void cmd_handler_init()
{
    cmd_build_cmd_hash_table();
    cmd_build_name_hash_table();
}

void cmd_handler_deinit()
{
    free(cmd_hash_table);
    free(name_hash_table);
}
