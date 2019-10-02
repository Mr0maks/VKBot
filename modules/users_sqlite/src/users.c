#include "users.h"
#include "dynamic_strings.h"
#include <stdio.h>
#include <enginecallbacks.h>

static db_handle_t *user_db = NULL;

int users_init(void)
{
  user_db = DB_OPEN("./users.sqlite3", "sqlite", NULL);

  if(!user_db)
    return 1;

  DB_EXEC(user_db, DB_CREATE_TABLE_MAIN, NULL, NULL);
  return 0;
}

static void user_new_user(int id)
{
    string_t s = STRING_INIT();

    STRING_FORMAT(s, DB_INSERT_USER, id);

    DB_EXEC(user_db, s->ptr, NULL, NULL);

    STRING_DESTROY(s);
}

static int privilage_callback(void *data, int argc, char **argv, char **colums_name)
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
  string_t s = STRING_INIT();

  STRING_FORMAT(s, DB_GET_PRIVILAGE_BY_ID, id);

  privilage_t ret = error;

  DB_EXEC(user_db, s->ptr, privilage_callback, &ret );

  STRING_DESTROY(s);

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
