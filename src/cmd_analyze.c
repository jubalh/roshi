// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h>
#include <stdio.h> // printf()
#include "sql.h"

void cmd_analyze(const char *filename)
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
