// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free()
#include <time.h> // localtime(), strftime()
#include <readline/readline.h>
#include "sql.h"

// Nr of SESSION_FIELDS
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

// Nr of EXIERCISE_FIELDS
#define MAX_E_ENUM_FIELDS 10

// Enum to access the readline buffer for the exercise
enum EXERCISE_FIELDS {
	EXNAME = 0,
	EXSETS,
	EXREPS,
	EXTIME,
	EXREST,
	EXWEIGHT,
	EXISWARMUP,
	EXNOTE,
	EXTEMPO,
	EXSTATION,
};

char * session_name_generator(const char *text, int state)
{
	static sqlite3_stmt *stmt = NULL;

	// first call
	if (!state) {
		char query[4096];
		query[0] = '\0';

		snprintf(query, 4095,  "SELECT Sessions.Name FROM Sessions WHERE Sessions.Name LIKE '%s%%'", text);

		int rc = sqlite3_prepare_v2(g_db, query, -1, &stmt, NULL );
		if( rc!=SQLITE_OK )
		{
			printf("error");
		}
	}

	// go through all results and return them
	const char *data = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		data = (const char*)sqlite3_column_text( stmt, 0 );
		return strdup(data);
	}

	return NULL;
}

char ** cmd_add_completion_none(const char *text, int start, int end)
{
	rl_attempted_completion_over = 1;
	return NULL;
}

char ** cmd_add_completion_session(const char *text, int start, int end)
{
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, session_name_generator);
}

int readline_time_template_hook(void) {
	time_t t;
	struct tm* tm_info;

	time(&t);
	tm_info = localtime(&t);

	char buffer[12];
	strftime(buffer, 12, "%H:%M:%S", tm_info);

	return rl_insert_text(buffer);
}

int readline_date_template_hook(void) {
	time_t t;
	struct tm* tm_info;

	time(&t);
	tm_info = localtime(&t);

	char buffer[12];
	strftime(buffer, 12, "%Y-%m-%d", tm_info);

	return rl_insert_text(buffer);
}

int readline_warmup_template_hook(void) {
	return rl_insert_text("no");
}

void cmd_add(const char *filename)
{
	char *bs[MAX_S_ENUM_FIELDS];

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
		bs[i] = NULL;

	printf("New Session\n");
	printf("-----------------------\n");

	open_db(filename);
	rl_attempted_completion_function = cmd_add_completion_session;

	// collect session info
	bs[SNAME] = readline("Session name: ");
	rl_attempted_completion_function = cmd_add_completion_none;
	bs[SPLACE] = readline("Place: ");

	rl_startup_hook = readline_date_template_hook;
	bs[SDATE] = readline("Date: ");
	rl_startup_hook = readline_time_template_hook;
	bs[SSTART] = readline("Start time: ");
	bs[SEND] = readline("End time: ");
	rl_startup_hook = NULL;

	bs[SNOTES] = readline("Notes: ");
	bs[SFEELING] = readline("Feeling: ");

	// submit session
	char query[4096];
	snprintf(query, 4095, "INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('%s', '%s', '%s %s', '%s %s')", bs[SNAME], bs[SPLACE], bs[SDATE], bs[SSTART], bs[SDATE], bs[SEND]);

	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, NULL))
		printf("error");

	// get session id
	sqlite3_int64 session_id = sqlite3_last_insert_rowid(g_db);

	// init exercise buffers
	char *be[MAX_E_ENUM_FIELDS];

	for (int i=0; i<MAX_E_ENUM_FIELDS; i++)
		be[i] = NULL;

	// collect exercise info
	printf("\nExercises:\n");

	be[EXNAME] = readline("Exercise name: ");
	while (strcmp(be[EXNAME], "") != 0)
	{
		be[EXSTATION] = readline("Station: ");
		be[EXWEIGHT] = readline("Weight: ");
		be[EXSETS] = readline("Sets: ");
		be[EXREPS] = readline("Reps: ");
		be[EXTIME] = readline("Time/Duration: ");
		be[EXTEMPO] = readline("Tempo: ");
		be[EXREST] = readline("Rest time: ");
		rl_startup_hook = readline_warmup_template_hook;
		be[EXISWARMUP] = readline("Warmup set: ");
		rl_startup_hook = NULL;
		be[EXNOTE] = readline("Notes: ");

		// submit exercise
		char *szErrMsg = 0;
		snprintf(query, 4095, "INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionID`) VALUES ('%s', '%s', '%s', '%s', '%d')", be[EXNAME], be[EXSETS], be[EXREPS], be[EXWEIGHT], session_id);
		if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, &szErrMsg))
		{
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		}

		for (int i=0; i<MAX_E_ENUM_FIELDS; i++)
		{
			free(be[i]);
		}
		be[EXNAME] = readline("\nExercise name: ");
	}

	// cleanup
	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(bs[i]);
	}

	close_db();
}
