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
#define MAX_E_ENUM_FIELDS 11

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
	EXTAGS
};

char * generic_generator(const char *table, const char *text, int state, const char *col)
{
	static sqlite3_stmt *stmt = NULL;

	// first call
	if (!state) {
		char query[4096];
		query[0] = '\0';

		snprintf(query, 4095,  "SELECT %s FROM %s WHERE %s LIKE '%s%%' GROUP BY %s", col, table, col, text, col);

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

char * session_name_generator(const char *text, int state)
{
	return generic_generator("Sessions", text, state, "Sessions.Name");
}

char * session_place_generator(const char *text, int state)
{
	return generic_generator("Sessions", text, state, "Sessions.Place");
}

char * exercise_tag_generator(const char *text, int state)
{
	return generic_generator("Tags", text, state, "Tags.Name");
}

char * exercise_name_generator(const char *text, int state)
{
	return generic_generator("Exercises", text, state, "Exercises.Name");
}

char ** cmd_add_completion_none(const char *text, int start, int end)
{
	rl_attempted_completion_over = 1;
	return NULL;
}

char ** cmd_add_completion_session_name(const char *text, int start, int end)
{
	// no default completion
	rl_attempted_completion_over = 1;
	// dont add space after completion
	rl_completion_append_character = '\0';

	return rl_completion_matches(text, session_name_generator);
}

char ** cmd_add_completion_session_place(const char *text, int start, int end)
{
	// no default completion
	rl_attempted_completion_over = 1;
	// dont add space after completion
	rl_completion_append_character = '\0';

	return rl_completion_matches(text, session_place_generator);
}

char ** cmd_add_completion_exercise_tag(const char *text, int start, int end)
{
	// no default completion
	rl_attempted_completion_over = 1;
	// dont add space after completion
	rl_completion_append_character = '\0';

	return rl_completion_matches(text, exercise_tag_generator);
}

char ** cmd_add_completion_exercise_name(const char *text, int start, int end)
{
	// no default completion
	rl_attempted_completion_over = 1;
	// dont add space after completion
	rl_completion_append_character = '\0';

	return rl_completion_matches(text, exercise_name_generator);
}

int readline_time_template_hook(void) {
	time_t t;
	struct tm* tm_info;

	time(&t);
	tm_info = localtime(&t);

	char buffer[12];
	strftime(buffer, 12, "%H:%M", tm_info);

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

int readline_feeling_template_hook(void) {
	return rl_insert_text("0");
}

void cmd_add(const char *filename)
{
	char *bs[MAX_S_ENUM_FIELDS];

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
		bs[i] = NULL;

	printf("New Session\n");
	printf("-----------------------\n");

	open_db(filename);

	rl_attempted_completion_function = cmd_add_completion_session_name;

	// collect session info
	bs[SNAME] = readline("Session name: ");
	rl_attempted_completion_function = cmd_add_completion_session_place;
	bs[SPLACE] = readline("Place: ");
	rl_attempted_completion_function = cmd_add_completion_none;

	rl_startup_hook = readline_date_template_hook;
	bs[SDATE] = readline("Date: ");
	rl_startup_hook = readline_time_template_hook;
	bs[SSTART] = readline("Start time: ");
	bs[SEND] = readline("End time: ");
	rl_startup_hook = NULL;

	bs[SNOTES] = readline("Notes: ");

	rl_startup_hook = readline_feeling_template_hook;
	bs[SFEELING] = readline("Feeling (-2 to 2): ");
	rl_startup_hook = NULL;

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

	rl_attempted_completion_function = cmd_add_completion_exercise_name;
	be[EXNAME] = readline("Exercise name: ");
	rl_attempted_completion_function = cmd_add_completion_none;
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
		rl_attempted_completion_function = cmd_add_completion_exercise_tag;
		be[EXTAGS] = readline("Tags: ");
		rl_attempted_completion_function = cmd_add_completion_none;

		// submit exercise
		char *szErrMsg = 0;
		snprintf(query, 4095, "INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionID`) VALUES ('%s', '%s', '%s', '%s', '%d')", be[EXNAME], be[EXSETS], be[EXREPS], be[EXWEIGHT], session_id);
		if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, &szErrMsg))
		{
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		}

		sqlite3_int64 exercise_id = sqlite3_last_insert_rowid(g_db);

		sqlite3_stmt *stmt_tag= NULL;
		sqlite3_stmt *stmt_tag_ex = NULL;

		sqlite3_prepare_v2(g_db, "INSERT INTO Tags (Name) SELECT :strtagname WHERE NOT EXISTS (SELECT TagId FROM Tags WHERE Name = :strtagname)", -1, &stmt_tag, NULL );

		sqlite3_prepare_v2(g_db, "INSERT INTO ExercisesTags (TagId, ExerciseId) VALUES(:strtagid, :strexid)", -1, &stmt_tag_ex, NULL );

		int id_strtagname = sqlite3_bind_parameter_index(stmt_tag, ":strtagname");
		int id_strtagid = sqlite3_bind_parameter_index(stmt_tag_ex, ":strtagid");
		int id_strexid = sqlite3_bind_parameter_index(stmt_tag_ex, ":strexid");

		char *tag = strtok(be[EXTAGS], ",");
		while(tag != NULL) {
			// insert new tag if not already present
			sqlite3_bind_text( stmt_tag, id_strtagname, tag, -1, SQLITE_STATIC );
			int rc = sqlite3_step(stmt_tag);
			sqlite3_reset(stmt_tag);

			if (rc == SQLITE_OK || rc == SQLITE_DONE)
			{
				sqlite3_int64 tag_id = sqlite3_last_insert_rowid(g_db);

				// insert tag to exercise connection into ExerciseTags
				sqlite3_bind_int( stmt_tag_ex, id_strtagid, tag_id );
				sqlite3_bind_int( stmt_tag_ex, id_strexid, exercise_id );
				sqlite3_step(stmt_tag_ex);
				sqlite3_reset(stmt_tag_ex);
			} else {
				fprintf(stderr, "Error in cmd_add(): add tags\n");
			}


			tag = strtok(NULL, ",");
		}

		sqlite3_finalize(stmt_tag);
		sqlite3_finalize(stmt_tag_ex);

		for (int i=0; i<MAX_E_ENUM_FIELDS; i++)
		{
			free(be[i]);
		}
		rl_attempted_completion_function = cmd_add_completion_exercise_name;
		be[EXNAME] = readline("\nExercise name: ");
		rl_attempted_completion_function = cmd_add_completion_none;
	}

	// cleanup
	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(bs[i]);
	}

	close_db();
}
