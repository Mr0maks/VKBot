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

typedef void *(*db_open_t)(void *db_data, void *data, const char *filename);
typedef void (*db_exec_t)(db_handle_t *db, const char *cmd, db_callback_t callback, void *data);
typedef void (*db_close_t)(db_handle_t *db);

void db_register_type( const char *type, db_open_t open, db_exec_t exec, db_close_t close, void *data );
db_handle_t *db_open(const char *filename, const char *db_name, void *data);
void db_exec(db_handle_t *db, const char *cmd, db_callback_t callback, void *data);
void db_close(db_handle_t *db);
