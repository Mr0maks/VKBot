#include <enginecallbacks.h>
#include "va_utils.h"
#include "db_core.h"

db_handle_t *chat_greetings = NULL;

int db_chat_greetings_init(void)
{
  if(chat_greetings)
      return true;

  chat_greetings = DB_OPEN("./greetings.sqlite3", "sqlite", NULL);

  if(!chat_greetings)
      return false;

  DB_EXEC(chat_greetings, DB_GREETINGS_CREATE_TABLE_MAIN, NULL, NULL);

  return true;
}

int db_chat_greetings_push(int peer_id, const char *args )
{
    if(!chat_greetings)
        return false;

    if(!peer_id || !args)
        return false;

    DB_EXEC(chat_greetings, va(DB_GREETINGS_INSERT_GREETING, peer_id, args), NULL, NULL );

    return true;
}

static int db_chat_greetings_callback(void *data, int argc, char **argv, char **colums_name)
{
    string_t *greeting = (string_t*)data;

    if(colums_name[0])
    {
        *greeting = STRING_INIT();
        STRING_COPY(*greeting, argv[0]);
        return 0;
    }

    return 0;
}

string_t db_chat_greetings_get(int peer_id )
{
    if(!peer_id)
        return NULL;

    string_t greeting = NULL;

    DB_EXEC(chat_greetings, va(DB_GREETINGS_GET_GREETING_BY_PEER_ID, peer_id), db_chat_greetings_callback, &greeting );

    return greeting;
}
