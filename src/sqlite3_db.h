#pragma once

#define DB_CREATE_TABLE_MAIN "CREATE TABLE IF NOT EXISTS users ( \
id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
user_id INTEGER NOT NULL, \
privilage INTERGER NOT NULL, \
json TEXT UNIQUE \
); \
"

#define DB_INSERT_USER "INSERT INTO users(user_id, privilage, json) VALUES (%i, 0, NULL);"

#define DB_GET_USER_BY_ID "SELECT * FROM users WHERE user_id = %i;"

#define DB_GET_PRIVILAGE_BY_ID "SELECT privilage FROM users WHERE user_id = %i;"
#define DB_SET_PRIVILAGE_BY_ID "UPDATE users SET privilage = %i WHERE user_id = %i;"


typedef int (*db_callback_t)(void *data, int argc, char **argv, char **colums);

typedef struct db_handle_s
{
    void (*db_exec)(struct db_handle_s *db, const char *cmd, db_callback_t callback, void *data);
    void (*db_close)(struct db_handle_s *db);
    void *db_handle;
} db_handle_t;

db_handle_t *db_sqlite_open(const char *filename);
void db_sqlite_exec(db_handle_t *db, const char *cmd, db_callback_t callback, void *data);
void db_sqlite_close(db_handle_t *db);
