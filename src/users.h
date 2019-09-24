#pragma once
#include "sqlite3_db.h"

typedef enum
{
  error = -2,
  banned,
  none,
  vip,
  admin
} privilage_t;

typedef struct
{
  int user_id;
  privilage_t privilage;
} users_privilage_t;

void users_set_db(db_handle_t *handle);

privilage_t get_privilage_by_id(int id);
const char *get_name_privilage(privilage_t priv);
