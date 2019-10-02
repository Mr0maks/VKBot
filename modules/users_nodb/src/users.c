#include "users.h"

const users_privilage_t users[] = {
  {212232274, admin},
  {0, none}
};

void user_init_void()
{
}

void user_new_user(int id)
{
}

privilage_t get_privilage_by_id(int id)
{
  for(int i = 0; users[i].user_id != 0; i++)
    {
      if(users[i].user_id == id)
	return users[i].privilage;
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

void users_unregister()
{
}
