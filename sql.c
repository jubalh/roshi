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

void create_example_db(char *name)
{
	char *szErrMsg;

	char *query[] = { 
		"CREATE TABLE `Exercises` ( `ExerciseId` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `Name` TEXT NOT NULL, `Sets` INTEGER DEFAULT 1, `Reps` INTEGER DEFAULT 1, `Time` TEXT, `Rest` TEXT, `Weight` NUMERIC DEFAULT 0, `isWarmup` INTEGER DEFAULT 0, `Note` TEXT, `Tempo` TEXT, `Station` TEXT, `SessionId` INTEGER, FOREIGN KEY(`SessionId`) REFERENCES `Session`(`SessionId`) )",

		"CREATE TABLE `Sessions` ( `SessionId` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `Name` TEXT NOT NULL, `Place` TEXT, `Start` NUMERIC, `End` TEXT, `Note` TEXT, `Feeling` INTEGER )",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Weightlifting', 'Leimen', '2018-05-23 11:00', '2018-05-23 12:00')",
		
		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Bodyweight', 'Sunnypark', '2018-05-24 09:00', '2018-05-23 09:30')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `SessionId`) VALUES ('Push-Ups', '2', '20', '2')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '5', '5', '80', '1')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '10', '10', '35', '1')" };


	for (int i=0; i<7; i++)
	{
		if( SQLITE_OK != sqlite3_exec(g_db, query[i], NULL, 0, &szErrMsg))
			goto error;
	}

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

void sql_submit(char *filename, char *query)
{
	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, NULL))
		printf("error");
}
