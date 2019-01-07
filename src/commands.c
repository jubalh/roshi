// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <readline/readline.h>
#include "sql.h"

void cmd_show(char *filename, char *session)
{
	char *szErrMsg = 0;
	sqlite3_stmt *stmt = NULL;

    open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT Exercises.Name, Sets, Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = :str;", -1, &stmt, NULL );
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
	int idx = sqlite3_bind_parameter_index(stmt, ":str");
    sqlite3_bind_text( stmt, idx, session, -1, SQLITE_STATIC );

	const char *name = NULL;
	const char *sets = NULL;
	const char *reps = NULL;
	const char *weight = NULL;

	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		name = (const char*)sqlite3_column_text( stmt, 0 );
		sets = (const char*)sqlite3_column_text( stmt, 1 );
		reps = (const char*)sqlite3_column_text( stmt, 2 );
		weight = (const char*)sqlite3_column_text( stmt, 3 );

		if (strcmp(weight, "0") == 0)
			printf("%s: %sx%s\n", name, sets, reps);
		else
			printf("%s: %skg %sx%s\n", name, weight, sets, reps);
	}

	sqlite3_finalize(stmt);
	close_db();

//	if (cb_called == 0) todo? or dont want that anyways?
//		printf("No such session: %s\n", session);
}

void cmd_list(char *filename)
{
	char *szErrMsg = 0;
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT SessionID, Name from Sessions;", -1, &stmt, NULL );
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

	const char *id = NULL;
	const char *name = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		id = (const char*)sqlite3_column_text( stmt, 0 );
		name = (const char*)sqlite3_column_text( stmt, 1 );
		printf("%s\t%s\n", id, name);
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

void cmd_pr(char *filename)
{
	char *szErrMsg = 0;
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT Exercises.Name, Weight FROM Exercises INNER JOIN Tags ON Exercises.ExerciseId=Tags.ExerciseId WHERE Tags.Name = 'PR'", -1, &stmt, NULL );
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

	const char *name = NULL;
	const char *weight = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		name = (const char*)sqlite3_column_text( stmt, 0 );
		weight = (const char*)sqlite3_column_text( stmt, 1 );
		printf( "%s %s\n", name, weight);
	}

	sqlite3_finalize(stmt);
	close_db();
}
