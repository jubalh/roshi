// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <readline/readline.h>

int cb_called = 0;

static void sql_submit(char *filename, char *query)
{
	sqlite3 *db;
	sqlite3_open(filename, &db);
	if( SQLITE_OK != sqlite3_exec(db, query, NULL, 0, NULL))
		printf("error");
	sqlite3_close(db);
}

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
	{
		printf("SQL error");
		exit(1);
	}
}

static int cmd_show_callback(void *NotUsed, int argc, char **argv, char **azColName){
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
	printf("roshi file_name newlog - Create new log\n");
	printf("roshi file_name show session_name - Display session\n");
}

static void cmd_newlog(char *filename)
{
	create_example_db(filename);
	exit(0);
}

static void cmd_show(char *filename, char *session)
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
	rc = sqlite3_exec(db, query, cmd_show_callback, 0, &szErrMsg);
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}

	if (cb_called == 0)
		printf("No such session: %s\n", session);

	sqlite3_close(db);
}

// Nr of SESSION_FIELDs
#define MAX_S_ENUM_FIELDS 7
// Enum to access the readline buffer for the session
enum SESSION_FIELDS {
	SNAME = 0,
	SPLACE,
	SDATE,
	SSTART,
	SEND,
	SNOTES,
	SFEELING
};

static void cmd_add(char *filename)
{
	char *b[MAX_S_ENUM_FIELDS];

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
		b[i] = NULL;

	printf("----------\n");
	printf("New Session\n");
	printf("----------\n");

	b[SNAME] = readline("Session name: ");
	b[SPLACE] = readline("Place: ");
	b[SDATE] = readline("Date: ");
	b[SSTART] = readline("Start time: ");
	b[SEND] = readline("End time: ");
	b[SNOTES] = readline("Notes: ");
	b[SFEELING] = readline("Feeling: ");

	char query[1024];
	snprintf(query, 1023, "INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('%s', '%s', '%s', '%s')", b[SNAME], b[SPLACE], "2018-09-28 10:00", "2018-08-28 11:00");

	sql_submit(filename, query);

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(b[i]);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		print_usage();
		exit(1);
	}

	if (strcmp(argv[2], "newlog") == 0)
	{
		cmd_newlog(argv[1]);
	} else {
		// if db doesnt exist or no further argument
		if ( (access(argv[1], F_OK) == -1) || argc < 4)
		{
			print_usage();
			exit(1);
		}

		if (strcmp(argv[2], "show") == 0)
		{
			cmd_show(argv[1], argv[3]);
		} else if (strcmp(argv[2], "add") == 0)
		{
			cmd_add(argv[1]);
		}
	}
}
