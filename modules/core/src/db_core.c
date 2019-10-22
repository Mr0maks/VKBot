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

  DB_EXEC(db_chat, DB_CHAT_GREETINGS_CREATE_TABLE_MAIN, NULL, NULL);
  DB_EXEC(db_chat, DB_CHAT_WARNINGS_CREATE_TABLE_MAX_WARNINGS, NULL, NULL);

  return true;
}

int db_chat_greetings_push(int peer_id, const char *args )
{
    if(!db_chat)
        return false;

    if(!peer_id || !args)

        return false;

    DB_EXEC(db_chat, va(DB_CHAT_GREETINGS_INSERT_GREETING, peer_id, peer_id, args), NULL, NULL );

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

    DB_EXEC(db_chat, va(DB_CHAT_GREETINGS_GET_GREETING_BY_PEER_ID, peer_id), db_chat_greetings_callback, &greeting );

    return greeting;
}

void db_chat_warnings_chat_init(int peer_id)
{
    DB_EXEC(db_chat, va(DB_CHAT_WARNINGS_CREATE_TABLE_CHAT, peer_id), NULL, NULL);
    DB_EXEC(db_chat, va(DB_CHAT_WARNINGS_SETUP_MAX, peer_id, 3), NULL, NULL);
}

void db_chat_warnings_set(int peer_id, int id, int warning)
{
    DB_EXEC(db_chat, va(DB_CHAT_WARNINGS_SET_WARNING, peer_id, peer_id, id, warning), NULL, NULL);
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
    DB_EXEC(db_chat, va(DB_CHAT_WARNINGS_GET_WARNING, peer_id, id), db_chat_warnings_get_callback, &warnings );
    return warnings;
}

void db_chat_warnings_inc(int peer_id, int id)
{
    int warnings = db_chat_warnings_get(peer_id, id);

    warnings++;

    db_chat_warnings_set(peer_id, id, warnings);
}

int db_chat_get_max_warnings(int peer_id)
{
    int max_warnings = 0;
    DB_EXEC(db_chat, va(DB_CHAT_WAGNINGS_GET_MAX, peer_id), db_chat_warnings_get_callback, &max_warnings );
    return max_warnings;
}

bool db_chat_warnings_is_kick(int peer_id, int id)
{
    return (db_chat_warnings_get(peer_id, id) == db_chat_get_max_warnings(peer_id));
}

void db_chat_privilage_user_init(int peer_id, int id)
{
    DB_EXEC(db_chat, va(DB_CHAT_PRIVILAGE_SETUP, peer_id, id, 0, 0), NULL, NULL);
}

void db_chat_set_privilage(int peer_id, int id, int priv)
{
    db_chat_get_privilage(peer_id, id);
    DB_EXEC(db_chat, va(DB_CHAT_SET_PRIVILAGE, peer_id, priv, id), NULL, NULL );
}

int db_chat_get_privilage(int peer_id, int id)
{
    int privilage = -3;

try_again:
    DB_EXEC(db_chat, va(DB_CHAT_GET_PRIVILAGE, peer_id, id), db_chat_warnings_get_callback, &privilage );

    if(privilage == -3)
    {
        db_chat_privilage_user_init(peer_id, id);
        goto try_again;
    }

    return privilage;
}
