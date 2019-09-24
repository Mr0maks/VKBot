#include "users.h"
#include "dynamic_strings.h"
#include "sqlite3_db.h"
#include <stdio.h>

const users_privilage_t users[] = {
//  {212232274, admin},
  {0, none}
};

static db_handle_t *user_db = NULL;

void users_set_db(db_handle_t *handle)
{
    user_db = handle;
}

void user_new_user(int id)
{
    string_t s = string_init();

    string_format(s, DB_INSERT_USER, id);

    db_sqlite_exec(user_db, s->ptr, NULL, NULL);

    string_destroy(s);
}

int privilage_callback(void *data, int argc, char **argv, char **colums_name)
{
       for(int i = 0; i<argc; i++){
          printf("%s = %s\n", colums_name[i], argv[i] ? argv[i] : "NULL");
       }

    privilage_t *priv = (privilage_t*)(data);

    if(colums_name[0])
        *priv = atoi(argv[0]);

       return 0;
}

privilage_t get_privilage_by_id(int id)
{
  for(int i = 0; users[i].user_id != 0; i++)
    {
      if(users[i].user_id == id)
	return users[i].privilage;
    }

  string_t s = string_init();

  string_format(s, DB_GET_PRIVILAGE_BY_ID, id);

  privilage_t ret = error;

  db_sqlite_exec(user_db, s->ptr, privilage_callback, &ret );

  string_destroy(s);

  if(ret != error)
      return ret;

  if(ret == error)
  {
  user_new_user(id);
  }

  return none;
}

const char *get_name_privilage(privilage_t priv)
{
  switch (priv) {
      case error: return "лох";
      case banned: return "забанненый";
      case none: return "обычный пользователь";
      case vip: return "vip пользователь";
      case admin: return "администратор";
    }
  return "неизвестно";
}
