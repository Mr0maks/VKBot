#pragma once

typedef enum
{
  error = -2,
  banned,
  none,
  vip,
  chat_admin,
  bot_super_admin
} privilage_t;

typedef struct
{
  int user_id;
  privilage_t privilage;
} users_privilage_t;

privilage_t get_privilage_by_id(int id);
const char *get_name_privilage(privilage_t priv);
