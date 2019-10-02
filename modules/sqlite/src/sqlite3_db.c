#include <sqlite3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sqlite3_db.h"

void *sqlite_open(void *db_data, void *data, const char *filename)
{
    sqlite3 *db_handle = NULL;

    int error_code = sqlite3_open(filename, &db_handle);

    if(error_code)
    {
        printf("Cant open db %s\nError: %s\n", filename, sqlite3_errmsg(db_handle));
        return NULL;
    }

    return db_handle;
}

void sqlite_exec(db_handle_t *db, const char *cmd, db_callback_t callback, void *data)
{
  char *error_msg = NULL;

  int error_code = sqlite3_exec(db->db_handle, cmd, callback, data, &error_msg);

  if( error_code != SQLITE_OK ){
      printf("SQL error: %s\n", error_msg);
      sqlite3_free(error_msg);
    }
}

void sqlite_close(struct db_handle_s *db)
{
  int error_code = sqlite3_close(db->db_handle);

  if(error_code)
    {
      printf("Cant close db\nError: %s\n", sqlite3_errmsg(db->db_handle));
      return;
    }
}
