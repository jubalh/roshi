// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h>
#include <stdio.h> // printf()
#include "sql.h"
#include <sys/stat.h> // mkdir()
#include <unistd.h> // access()

#define TMPDIR "/tmp/roshi/"
#define TMPFILE TMPDIR"f1.data"

void cmd_analyze(const char *filename)
{
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT Exercises.Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = 'Weightlifting';", -1, &stmt, NULL );
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
			exit(2);
	}

	FILE *fp;
	fp = fopen(TMPFILE, "w+");
	if (fp == NULL)
		exit(2);

	fputs("# reps, weight\n", fp);
	const char *reps = NULL;
	const char *weight = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		reps = (const char*)sqlite3_column_text( stmt, 0 );
		weight = (const char*)sqlite3_column_text( stmt, 1 );
		fprintf(fp, "%s %s\n", reps, weight);
	}

	sqlite3_finalize(stmt);
	close_db();

	fclose(fp);
	
	static char * call = "gnuplot -e \"set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7 pointsize 1.5; set xlabel 'reps'; set ylabel 'weight'; plot '/tmp/roshi/f1.data' with linespoints linestyle 1; pause -1\"";

	system(call);
}
