#include <sqlite3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sqlite3_db.h"

void sqlite_exec(struct db_handle_s *db, const char *cmd, db_callback_t callback, void *data)
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

void *sqlite_open(const char *filename)
{
    sqlite3 *db_handle = NULL;

    int error_code = sqlite3_open(filename, &db_handle);

    if(error_code)
    {
        printf("Cant open db %s\nError: %s\n", filename, sqlite3_errmsg(db_handle));
        return NULL;
    }

    db_handle_t *handle = (db_handle_t*)malloc(sizeof(db_handle_t));

    assert(handle);

    handle->db_exec = sqlite_exec;
    handle->db_close = sqlite_close;
    handle->db_handle = db_handle;

    return handle;
}

void db_sqlite_exec(db_handle_t *db, const char *cmd, db_callback_t callback, void *data)
{
    assert(db);
    assert(cmd);

    sqlite_exec(db, cmd, callback, data);
}

db_handle_t *db_sqlite_open(const char *filename)
{
    assert(filename);

    return sqlite_open(filename);
}

void db_sqlite_close(db_handle_t *db)
{
    assert(db);

    sqlite_close(db);
}
