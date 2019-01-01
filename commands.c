// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <sqlite3.h>
#include <readline/readline.h>
#include "sql.h"

static int cb_called = 0;

static int cmd_show_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	cb_called = 1;

	if (strcmp(argv[3], "0") == 0)
		printf("%s: %sx%s\n", argv[0], argv[1], argv[2]);
	else
		printf("%s: %skg %sx%s\n", argv[0], argv[3], argv[1], argv[2]);

	return 0;
}

void cmd_show(char *filename, char *session)
{
	char query[256];
	query[0] = '\0';

	sprintf(query, "SELECT Exercises.Name, Sets, Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = '%s';", session);

    open_db(filename);

	char *szErrMsg = 0;
	int rc = sqlite3_exec(g_db, query, cmd_show_callback, 0, &szErrMsg);
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}

	if (cb_called == 0)
		printf("No such session: %s\n", session);

	close_db();
}

void cmd_list(char *filename)
{
	char *szErrMsg = 0;
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT Sessions.Name FROM Sessions", -1, &stmt, NULL );
	if( rc!=SQLITE_OK )
	{
		if (szErrMsg) {
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		} else {
			printf("SQL error: %s\n", szErrMsg);
		}
		close_db();
		exit(1);
	}

	printf("List of sessions:\n");
	printf("-----------------------\n");

	const char *data = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		data = (const char*)sqlite3_column_text( stmt, 0 );
		printf( "%s\n", data ? data : "[NULL]" );
	}

	sqlite3_finalize(stmt);
	close_db();
}

void cmd_analyze(char *filename)
{
	char *szErrMsg = 0;
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT Exercises.Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = 'Weightlifting';", -1, &stmt, NULL );
	if( rc!=SQLITE_OK )
	{
		if (szErrMsg) {
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		} else {
			printf("SQL error: %s\n", szErrMsg);
		}
		close_db();
		exit(1);
	}

	const char *reps = NULL;
	const char *weight = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		reps = (const char*)sqlite3_column_text( stmt, 0 );
		weight = (const char*)sqlite3_column_text( stmt, 1 );
		printf( "%s %s\n", reps, weight);
	}

	sqlite3_finalize(stmt);
	close_db();
}
