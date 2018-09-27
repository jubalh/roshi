// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

int cb = 0;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	cb = 1;
	if (strcmp(argv[3], "0") == 0)
		printf("%s: %sx%s\n", argv[0], argv[1], argv[2]);
	else
		printf("%s: %skg %sx%s\n", argv[0], argv[3], argv[1], argv[2]);

	return 0;
}

int main(int argc, char *argv[])
{
	sqlite3 *db;

	if (argc < 3)
	{
		printf("Usage: test file_name session_name\n");
		return 1;
	}

    int status = sqlite3_open(argv[1], &db);
	if (status)
	{
		printf("Error");
		sqlite3_close(db);
		return 1;
	}

	char query[256];
	query[0] = '\0';

	sprintf(query, "SELECT Exercise.Name, \"Set\", \"Reps\", Weight FROM Exercise INNER JOIN Session ON Exercise.SessionId=Session.Id WHERE Session.Name = '%s';", argv[2]);

	char *szErrMsg = 0;
	status = sqlite3_exec(db, query, callback, 0, &szErrMsg);
	if( status!=SQLITE_OK )
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}

	if (cb == 0)
		printf("No such session: %s\n", argv[2]);

	sqlite3_close(db);
}
