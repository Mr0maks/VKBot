#pragma once

#include <db_api.h>

void *sqlite_open(void *db_data, void *data, const char *filename);
void sqlite_exec(db_handle_t *db, const char *cmd, db_callback_t callback, void *data);
void sqlite_close(db_handle_t *db);
