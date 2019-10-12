#pragma once

#include <enginecallbacks.h>

extern db_handle_t *chat_greetings;

int db_chat_greetings_init(void);
int db_chat_greetings_push(int peer_id, const char *args );
string_t db_chat_greetings_get(int peer_id );

#define DB_GREETINGS_CREATE_TABLE_MAIN "CREATE TABLE IF NOT EXISTS chat_greetings ( \
id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
peer_id INTEGER NOT NULL, \
text TEXT NOT NULL \
); \
"

#define DB_GREETINGS_INSERT_GREETING "INSERT OR REPLACE INTO chat_greetings( id, peer_id, text ) VALUES ( (SELECT id FROM chat_greetings WHERE peer_id = %i), %i, '%s' );"

#define DB_GREETINGS_GET_GREETING_BY_PEER_ID "SELECT text FROM chat_greetings WHERE peer_id = %i;"
