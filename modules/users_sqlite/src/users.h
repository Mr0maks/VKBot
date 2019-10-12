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

typedef enum
{
  error = -2,
  banned,
  none,
  vip,
  admin
} privilage_t;

int users_init(void);
privilage_t get_privilage_by_id(int id);
void set_privilage_by_id(int id, int priv);
const char *get_name_privilage(privilage_t priv);
