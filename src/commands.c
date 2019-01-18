// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <stdio.h> // printf()
#include <string.h> // strcmp()
#include "sql.h"

void cmd_show(const char *filename, const char *session, int isindex)
{
	int rc;
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	if (isindex)
	{
		// search for index
		rc = sqlite3_prepare_v2(g_db, "SELECT Exercises.Name, Sets, Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.SessionId = :str;", -1, &stmt, NULL );
	} else {
		// search for session name
		rc = sqlite3_prepare_v2(g_db, "SELECT Exercises.Name, Sets, Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = :str;", -1, &stmt, NULL );
	}

	if( rc!=SQLITE_OK )
	{
		printf("SQL error: cmd_show, code %d\n", rc);
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

void cmd_list(const char *filename)
{
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT SessionID, Name from Sessions;", -1, &stmt, NULL );
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: cmd_list, code %d\n", rc);
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

void cmd_pr(const char *filename)
{
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT e.Name, e.Weight, s.Start FROM Exercises e JOIN ExercisesTags et ON e.ExerciseId = et.ExerciseId JOIN Tags t ON t.TagId = et.TagId INNER JOIN Sessions s ON s.SessionId=e.SessionId WHERE t.name = 'PR' ORDER BY s.Start DESC;", -1, &stmt, NULL );
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: cmd_list, code %d\n", rc);
		close_db();
		exit(1);
	}

	const char *name = NULL;
	const char *weight = NULL;
	const char *date = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		name = (const char*)sqlite3_column_text( stmt, 0 );
		weight = (const char*)sqlite3_column_text( stmt, 1 );
		date = (const char*)sqlite3_column_text( stmt, 2 );
		printf( "%s %s %s\n", date, name, weight);
	}

	sqlite3_finalize(stmt);
	close_db();
}
