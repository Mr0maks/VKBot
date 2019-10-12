#pragma once

#ifndef _VKBOT_MODULE
typedef enum
{
  error = -2,
  banned,
  none,
  vip,
  admin
} privilage_t;
#endif

typedef struct
{
  int (*init)();
  int (*get_privilage)(int id);
  void (*set_privilage)(int id, int priv);
  const char *(*get_privilage_name)(int priv);
} users_t;

#ifndef _VKBOT_MODULE
int users_is_init();
void users_module_register_users(users_t *users);
privilage_t users_get_privilage(int id);
void users_set_privilage(int id, int priv);
const char *users_get_name_privilage(privilage_t priv);
#endif
