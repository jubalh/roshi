// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <stdio.h> // printf()
#include <sqlite3.h>

sqlite3 *g_db;

void open_db(char *filename)
{
	if (SQLITE_OK != sqlite3_initialize() ||
		sqlite3_open(filename, &g_db))
	{
		printf("SQL error");
		exit(1);
	}
}

void close_db()
{
	sqlite3_close(g_db);
	sqlite3_shutdown();
}

void sql_submit(char *filename, char *query)
{
	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, NULL))
		printf("error");
}
