#include <enginecallbacks.h>
#include "va_utils.h"
#include "db_core.h"

db_handle_t *db_chat = NULL;

int db_chat_init(void)
{
  if(db_chat)
      return true;

  db_chat = DB_OPEN("./chat.sqlite3", "sqlite", NULL);

  if(!db_chat)
      return false;

  DB_EXEC(db_chat, DB_GREETINGS_CREATE_TABLE_MAIN, NULL, NULL);
  DB_EXEC(db_chat, DB_WARNINGS_CREATE_TABLE_MAX_WARNINGS, NULL, NULL);

  return true;
}

int db_chat_greetings_push(int peer_id, const char *args )
{
    if(!db_chat)
        return false;

    if(!peer_id || !args)
        return false;

    DB_EXEC(db_chat, va(DB_GREETINGS_INSERT_GREETING, peer_id, peer_id, args), NULL, NULL );

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

    DB_EXEC(db_chat, va(DB_GREETINGS_GET_GREETING_BY_PEER_ID, peer_id), db_chat_greetings_callback, &greeting );

    return greeting;
}

void db_chat_warnings_chat_init(int peer_id)
{
    DB_EXEC(db_chat, va(DB_WARNINGS_CREATE_TABLE_CHAT, peer_id), NULL, NULL);
    DB_EXEC(db_chat, va(DB_WAGNINGS_SET_MAX, peer_id, peer_id, 3), NULL, NULL);
}

void db_chat_warnings_set(int peer_id, int id, int warning)
{
    DB_EXEC(db_chat, va(DB_WARNINGS_SET_WARNING, peer_id, peer_id, id, warning), NULL, NULL);
}

static int db_chat_warnings_get_callback(void *data, int argc, char *argv[], char *colums_name[])
{
    for(int i = 0; i<argc; i++){
       printf("%s = %s\n", colums_name[i], argv[i] ? argv[i] : "NULL");
    }

    if(colums_name[0])
    {
        *(int*)data = atoi(argv[0]);
        return 0;
    }
    return 0;
}

int db_chat_warnings_get(int peer_id, int id)
{
    int warnings = 0;
    DB_EXEC(db_chat, va(DB_WARNINGS_GET_WARNING, peer_id, id), db_chat_warnings_get_callback, &warnings );
    return warnings;
}

void db_chat_warnings_inc(int peer_id, int id)
{
    int warnings = db_chat_warnings_get(peer_id, id);

    warnings++;

    db_chat_warnings_set(peer_id, id, warnings);
}

bool db_chat_warning_is_kick(int peer_id, int id)
{

}
