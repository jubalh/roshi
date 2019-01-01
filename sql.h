// vim: noexpandtab:ts=4:sts=4:sw=4

#pragma once

#include <sqlite3.h>

extern sqlite3 *g_db;

void open_db(char *filename);
void close_db();
void create_example_db(char *name);
void sql_submit(char *filename, char *query);
