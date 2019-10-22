#pragma once

#include <enginecallbacks.h>
#include <stdbool.h>

extern db_handle_t *db_chat;

int db_chat_init(void);

int db_chat_greetings_push(int peer_id, const char *args );
string_t db_chat_greetings_get(int peer_id );

#define DB_CHAT_GREETINGS_CREATE_TABLE_MAIN "CREATE TABLE IF NOT EXISTS chat_greetings ( \
id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
peer_id INTEGER NOT NULL, \
text TEXT NOT NULL \
); \
"

#define DB_CHAT_GREETINGS_INSERT_GREETING "INSERT OR REPLACE INTO chat_greetings( id, peer_id, text ) VALUES ( (SELECT id FROM chat_greetings WHERE peer_id = %i), %i, '%s' );"
#define DB_CHAT_GREETINGS_GET_GREETING_BY_PEER_ID "SELECT text FROM chat_greetings WHERE peer_id = %i;"

void db_chat_warnings_chat_init(int peer_id);
void db_chat_warnings_set(int peer_id, int id, int warning);
int db_chat_warnings_get(int peer_id, int id);
void db_chat_warnings_inc(int peer_id, int id);
int db_chat_get_max_warnings(int peer_id);
bool db_chat_warnings_is_kick(int peer_id, int id);

#define DB_CHAT_WARNINGS_CREATE_TABLE_MAX_WARNINGS "CREATE TABLE IF NOT EXISTS chat_max_warnings ( \
id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
peer_id INTEGER NOT NULL, \
max_warnings INTEGER NOT NULL \
); \
"

#define DB_CHAT_WARNINGS_CREATE_TABLE_CHAT "CREATE TABLE IF NOT EXISTS chat_%i ( \
id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
user_id INTEGER NOT NULL, \
privilage INTERGER_NOT_NULL, \
warnings INTEGER NOT NULL \
); \
"

#define DB_CHAT_WARNINGS_SET_WARNING "UPDATE chat_%i SET warnings = %i WHERE user_id = %i;"
#define DB_CHAT_WARNINGS_GET_WARNING "SELECT warnings FROM chat_%i WHERE user_id = %i;"

#define DB_CHAT_WARNINGS_SETUP_MAX "INSERT INTO chat_max_warnings( peer_id, max_warnings ) VALUES ( %i, %i );"
#define DB_CHAT_WARNINGS_SET_MAX "UPDATE chat_max_warnings SET max_warnings = %i WHERE peer_id = %i;"
#define DB_CHAT_WAGNINGS_GET_MAX "SELECT max_warnings FROM chat_max_warnings WHERE peer_id = %i"

void db_chat_privilage_user_init(int peer_id, int id);
void db_chat_set_privilage(int peer_id, int id, int priv);
int db_chat_get_privilage(int peer_id, int id);

#define DB_CHAT_PRIVILAGE_SETUP "INSERT INTO chat_%i( user_id, privilage, warnings ) VALUES ( %i, %i, %i );"
#define DB_CHAT_SET_PRIVILAGE "UPDATE chat_%i SET privilage = %i WHERE user_id = %i;"
#define DB_CHAT_GET_PRIVILAGE "SELECT privilage FROM chat_%i WHERE user_id = %i;"
