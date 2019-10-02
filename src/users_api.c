#include "users_api.h"
#include <stdlib.h>
#include <stdio.h>

static users_t *users_db = NULL;

int users_is_init()
{
  return (users_db != NULL);
}

void users_module_register_users(users_t *users)
{
  if(!users_is_init())
    {
      if(users->init)
	{
	  if(users->init() == 0)
	  {
	      users_db = users;
	  } else {
	    printf("[USERS] Error: Module returned error\n");
	  }
	} else {
	  users_db = users;
	}
    } else {
      printf("[USERS] Warning: Cannot register user module twice\n");
      return;
    }
}

privilage_t users_get_privilage(int id)
{
  return users_db->get_privilage(id);
}

const char *users_get_name_privilage(privilage_t priv)
{
  return users_db->get_privilage_name(priv);
}
