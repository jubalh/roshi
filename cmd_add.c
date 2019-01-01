// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free()
#include <readline/readline.h>
#include "sql.h"

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
	char *b[MAX_S_ENUM_FIELDS];

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
		b[i] = NULL;

	printf("New Session\n");
	printf("-----------------------\n");

    open_db(filename);
	rl_attempted_completion_function = cmd_add_completion_session;

	b[SNAME] = readline("Session name: ");
	rl_attempted_completion_function = cmd_add_completion_none;
	b[SPLACE] = readline("Place: ");
	b[SDATE] = readline("Date: ");
	b[SSTART] = readline("Start time: ");
	b[SEND] = readline("End time: ");
	b[SNOTES] = readline("Notes: ");
	b[SFEELING] = readline("Feeling: ");

	char query[1024];
	snprintf(query, 1023, "INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('%s', '%s', '%s', '%s')", b[SNAME], b[SPLACE], "2018-09-28 10:00", "2018-08-28 11:00");

	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, NULL))
		printf("error");

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(b[i]);
	}

	close_db();
}
