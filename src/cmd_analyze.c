/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdlib.h> // exit()
#include <stdio.h> // printf()
#include <sys/stat.h> // mkdir()
#include <unistd.h> // access()
#include "sql.h"

#define TMPDIR "/tmp/roshi/"
#define TMPFILE TMPDIR"f1.data"

static void generic_two_data_callback(FILE *fp, sqlite3_stmt *stmt)
{
	fputs("# reps, weight\n", fp);
	const char *reps = NULL;
	const char *weight = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		reps = (const char*)sqlite3_column_text( stmt, 0 );
		weight = (const char*)sqlite3_column_text( stmt, 1 );
		fprintf(fp, "%s %s\n", reps, weight);
	}
}

static void cmd_analyze_generic(const char* filename, const char* query, void (*data_cb)(FILE*, sqlite3_stmt*), const char* call)
{
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, query, -1, &stmt, NULL );
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: cmd_analzye\n");
		close_db();
		exit(1);
	}

	if (access(TMPDIR, F_OK) < 0)
	{
		int ret = mkdir(TMPDIR, S_IRUSR | S_IWUSR | S_IXUSR);
		if (ret != 0)
		{
			exit(2);
		}
	}

	FILE *fp;
	fp = fopen(TMPFILE, "w+e");
	if (fp == NULL)
	{
		exit(2);
	}

	data_cb(fp, stmt);

	sqlite3_finalize(stmt);
	close_db();

	fclose(fp);
	
	system(call);
}

void cmd_analyze_sub_test1(const char* filename)
{
	static char* gpcall = "gnuplot -e \"set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7 pointsize 1.5; set xlabel 'reps'; set ylabel 'weight'; plot '/tmp/roshi/f1.data' with linespoints linestyle 1; pause -1\"";

	cmd_analyze_generic(filename, "SELECT Exercises.Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = 'Weightlifting'", &generic_two_data_callback, gpcall);
}

void cmd_analyze_sub_pr(const char* filename)
{
	static char* gpcall = "gnuplot -e \"set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7 pointsize 1.5; set xlabel 'date'; set xdata time; set timefmt '%Y-%m-%d'; set ylabel 'weight'; plot '/tmp/roshi/f1.data' using 1:3 with linespoints linestyle 1; pause -1\"";

	cmd_analyze_generic(filename,
			"SELECT s.Start, e.Weight FROM Exercises e JOIN ExercisesTags et ON e.ExerciseId = et.ExerciseId JOIN Tags t ON t.TagId = et.TagId INNER JOIN Sessions s ON s.SessionId=e.SessionId WHERE t.name = 'PR' ORDER BY s.Start DESC;"
			,&generic_two_data_callback,
			gpcall);
}

void cmd_analyze()
{
	printf("Analyze:\n");
	printf("pr\t Create graph with PRs over time\n");
	printf("test1\t Create graph with reps and weight\n");
}
