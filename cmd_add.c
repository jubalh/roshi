// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free()
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

static int cmd_add_session_completion_callback(void *buffer, int argc, char **argv, char **azColName)
{
	char *b = (char*)buffer;
	if (b[0] == '\0')
		strcpy(b, argv[0]);
	return 0;
}

char * session_name_generator(const char *text, int state)
{
    char *name;
	static char completion_buffer[1024];

	// first call
	if (!state) {
		completion_buffer[0] = '\0';
		char query[256];
		query[0] = '\0';

		sprintf(query, "SELECT Sessions.Name FROM Sessions WHERE Sessions.Name LIKE '%s%%'", text);

		char *szErrMsg = 0;
		int rc = sqlite3_exec(g_db, query, cmd_add_session_completion_callback, completion_buffer, &szErrMsg);
		if( rc!=SQLITE_OK )
		{
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		}
    }

	if (completion_buffer[0] != '\0')
	{
		char *s = strdup(completion_buffer);
		completion_buffer[0] = '\0';
		return s;
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

void cmd_add(char *filename)
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

	/* TODO: remove later. For easier development
	bs[SDATE] = readline("Date: ");
	bs[SSTART] = readline("Start time: ");
	bs[SEND] = readline("End time: ");
	*/
	bs[SDATE] = strdup("2018-01-01");
	bs[SSTART] = strdup("09:00");
	bs[SEND] = strdup("09:30");

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
	be[EXSTATION] = readline("Station: ");
	be[EXNAME] = readline("Exercise name: ");
	be[EXWEIGHT] = readline("Weight: ");
	be[EXSETS] = readline("Sets: ");
	be[EXREPS] = readline("Reps: ");
	be[EXTIME] = readline("Time/Duration: ");
	be[EXTEMPO] = readline("Tempo: ");
	be[EXREST] = readline("Rest time: ");
	be[EXISWARMUP] = readline("Warmup set: ");
	be[EXNOTE] = readline("Notes: ");

	// submit exercise
	char *szErrMsg = 0;
	snprintf(query, 4095, "INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionID`) VALUES ('%s', '%s', '%s', '%s', '%d')", be[EXNAME], be[EXSETS], be[EXREPS], be[EXWEIGHT], session_id);
	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, &szErrMsg))
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}

	// cleanup
	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(bs[i]);
	}
	for (int i=0; i<MAX_E_ENUM_FIELDS; i++)
	{
		free(be[i]);
	}

	close_db();
}
