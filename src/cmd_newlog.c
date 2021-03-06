/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdlib.h> // exit()
#include <stdio.h> // printf()
#include "sql.h"

static void create_example_db(int dummydata)
{
	char *szErrMsg;

	#define BASIC_QUERIES 4
	#define ALL_QUERIES  17

	char *query[] = { 
		"CREATE TABLE `Sessions` ( `SessionId` INTEGER PRIMARY KEY, `Name` TEXT NOT NULL, `Place` TEXT, `Start` TEXT, `End` TEXT, `Note` TEXT, `Feeling` INTEGER )",

		"CREATE TABLE `Exercises` ( `ExerciseId` INTEGER PRIMARY KEY, `Name` TEXT NOT NULL, `Sets` INTEGER DEFAULT 1, `Reps` INTEGER DEFAULT 1, `Time` TEXT, `Rest` TEXT, `Weight` NUMERIC DEFAULT 0, `Distance` NUMERIC DEFAULT 0, `isWarmup` INTEGER DEFAULT 0, `Note` TEXT, `Tempo` TEXT, `Station` TEXT, `SessionId` INTEGER, FOREIGN KEY(`SessionId`) REFERENCES `Session`(`SessionId`) )",

		"CREATE TABLE `Tags` ( `TagId` INTEGER PRIMARY KEY, `Name` TEXT NOT NULL )",

		"CREATE TABLE `ExercisesTags` ( `TagId` INTEGER NOT NULL, `ExerciseId` INTEGER NOT NULL, FOREIGN KEY(`TagId`) REFERENCES `Tags`(`TagId`), FOREIGN KEY('ExerciseId') REFERENCES `Exercises`(`ExerciseId`) )",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Weightlifting', 'Leimen', '2018-05-23 11:00', '2018-05-23 12:00')",
		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '5', '5', '80', '1')",
		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Deadlift', '1', '1', '120', '1')",
		"INSERT INTO `Tags` (`Name`) VALUES ('PR')",
		"INSERT INTO `ExercisesTags` (`TagId`, `ExerciseId`) VALUES ('1','2')",
		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '10', '10', '35', '1')",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Weightlifting', 'Leimen', '2018-06-23 15:00', '2018-05-23 15:30')",
		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '1', '1', '150', '2')",
		"INSERT INTO `ExercisesTags` (`TagId`, `ExerciseId`) VALUES ('1','4')",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Bodyweight', 'Sunnypark', '2018-05-24 09:00', '2018-05-24 09:30')",
		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `SessionId`) VALUES ('Push-Ups', '2', '20', '3')",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Swimming', 'OceanSpot1', '2018-08-08 08:00', '2018-08-08 08:30')",
		"INSERT INTO `Exercises` (`Name`, `Distance`, `SessionId`) VALUES ('Swimming', '200', '4')"
	};

	int queries_count = BASIC_QUERIES;
	if (dummydata)
		queries_count = ALL_QUERIES;

	for (int i=0; i < queries_count; i++)
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

void cmd_newlog(const char *filename, int dummydata)
{
	open_db(filename);
	create_example_db(dummydata);
	close_db();
	exit(0);
}
