#pragma once

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
const char *get_name_privilage(privilage_t priv);
