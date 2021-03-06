#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

#include "db_api.h"

typedef struct db_pool_s
{
  const char *type;

  void *(*db_open)(void *db_data, void *data, const char *filename);
  void (*db_exec)(struct db_handle_s *db, const char *cmd, db_callback_t callback, void *data);
  void (*db_close)(struct db_handle_s *db);

  void *data;

  struct db_pool_s *next;
} db_pool_t;

static db_pool_t *pool = NULL;

static inline db_pool_t *db_find(const char *name)
{
  assert(name);

  db_pool_t *ptr = pool;

  while (ptr != NULL) {
      if(!strncasecmp(ptr->type, name, strlen(ptr->type)))
	return ptr;

      ptr = ptr->next;
    }

  return NULL;
}

static void db_register( const char *type, db_open_t open, db_exec_t exec, db_close_t close, void *data )
{
  assert(type);

  printf("[DB] Registered Type DB: %s\n", type);

  db_pool_t *ptr = malloc(sizeof (db_pool_t));

  if(!ptr)
    return;

  ptr->type = type;
  ptr->db_open = open;
  ptr->db_exec = exec;
  ptr->db_close = close;
  ptr->data = data;

  ptr->next = pool;
  pool = ptr;
}

static db_handle_t *db_open(const char *filename, const char *db_name, void *data)
{
    assert(filename);
    assert(db_name);

    db_pool_t *ptr = db_find(db_name);

    if(!ptr)
      return NULL;

    db_handle_t *handle = malloc(sizeof(db_handle_t));

    if(!handle)
      return NULL;

    handle->db_exec = ptr->db_exec;
    handle->db_close = ptr->db_close;
    handle->db_handle = ptr->db_open(ptr->data, data, filename);

    return handle;
}

static void db_exec(db_handle_t *db, const char *cmd, db_callback_t callback, void *data)
{
  assert(db);
  assert(cmd);

  if(db->db_exec)
    db->db_exec(db, cmd, callback, data);
}

static void db_close(db_handle_t *db)
{
  assert(db);

  if(db->db_close)
    db->db_close(db);
}

void db_get_api(int ifver, db_api_t *api)
{
    assert(ifver == DB_API_VERSION);
    assert(api);

    api->db_register = db_register;
    api->db_open = db_open;
    api->db_exec = db_exec;
    api->db_close = db_close;
}
