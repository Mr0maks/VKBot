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
  const char *(*get_privilage_name)(int priv);
} users_t;

#ifndef _VKBOT_MODULE
int users_is_init();
void users_module_register_users(users_t *users);
privilage_t users_get_privilage(int id);
const char *users_get_name_privilage(privilage_t priv);
#endif
