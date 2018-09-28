// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

int cb_called = 0;

static void create_example_db(char *name)
{
	sqlite3 *db;
	char *szErrMsg;

	char *query[] = { 
		"CREATE TABLE `Exercises` ( `ExerciseId` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `Name` TEXT NOT NULL, `Sets` INTEGER DEFAULT 1, `Reps` INTEGER DEFAULT 1, `Time` TEXT, `Rest` TEXT, `Weight` NUMERIC DEFAULT 0, `isWarmup` INTEGER DEFAULT 0, `Note` TEXT, `Tempo` TEXT, `Station` TEXT, `SessionId` INTEGER, FOREIGN KEY(`SessionId`) REFERENCES `Session`(`SessionId`) )",

		"CREATE TABLE `Sessions` ( `SessionId` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `Name` TEXT NOT NULL, `Place` TEXT, `Start` NUMERIC, `End` TEXT, `Note` TEXT, `Feeling` INTEGER )",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Weightlifting', 'Leimen', '2018-05-23 11:00', '2018-05-23 12:00')",
		
		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Bodyweight', 'Sunnypark', '2018-05-24 09:00', '2018-05-23 09:30')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `SessionId`) VALUES ('Push-Ups', '2', '20', '2')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '5', '5', '80', '1')" };

	if (sqlite3_open(name, &db))
		goto error;

	for (int i=0; i<6; i++)
	{
		if( SQLITE_OK != sqlite3_exec(db, query[i], NULL, 0, &szErrMsg))
			goto error;
	}

	sqlite3_close(db);
	return;

error:
	if (szErrMsg)
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}
	else
		printf("SQL error");
		exit(1);
}

static int db_operation_callback(void *NotUsed, int argc, char **argv, char **azColName){
	cb_called = 1;

	if (strcmp(argv[3], "0") == 0)
		printf("%s: %sx%s\n", argv[0], argv[1], argv[2]);
	else
		printf("%s: %skg %sx%s\n", argv[0], argv[3], argv[1], argv[2]);

	return 0;
}

static void print_usage()
{
	printf("Usage:\n");
	printf("roshi newlog file_name - Create new log\n");
	printf("roshi file_name session_name - Display session\n");
}

static void cmd_newlog(char *filename)
{
	create_example_db(filename);
	exit(0);
}

static void db_operation(char *filename, char *session)
{
	sqlite3 *db;
	int rc = sqlite3_open(filename, &db);
	if (rc)
	{
		printf("Error");
		sqlite3_close(db);
		exit(1);
	}

	char query[256];
	query[0] = '\0';

	sprintf(query, "SELECT Exercises.Name, Sets, Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = '%s';", session);

	char *szErrMsg = 0;
	rc = sqlite3_exec(db, query, db_operation_callback, 0, &szErrMsg);
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}

	if (cb_called == 0)
		printf("No such session: %s\n", session);

	sqlite3_close(db);
}

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		print_usage();
		exit(1);
	}

	if (strcmp(argv[1], "newlog") == 0)
	{
		cmd_newlog(argv[2]);
	} else {
		db_operation(argv[1], argv[2]);
	}
}
