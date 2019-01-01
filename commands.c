// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <sqlite3.h>
#include <readline/readline.h>

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

static int cb_called = 0;
static sqlite3 *g_db = NULL;

static void open_db(char *filename)
{
	if (SQLITE_OK != sqlite3_initialize() ||
		sqlite3_open(filename, &g_db))
	{
		printf("SQL error");
		exit(1);
	}
}

static void close_db()
{
	sqlite3_close(g_db);
	sqlite3_shutdown();
}

static void create_example_db(char *name)
{
	char *szErrMsg;

	char *query[] = { 
		"CREATE TABLE `Exercises` ( `ExerciseId` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `Name` TEXT NOT NULL, `Sets` INTEGER DEFAULT 1, `Reps` INTEGER DEFAULT 1, `Time` TEXT, `Rest` TEXT, `Weight` NUMERIC DEFAULT 0, `isWarmup` INTEGER DEFAULT 0, `Note` TEXT, `Tempo` TEXT, `Station` TEXT, `SessionId` INTEGER, FOREIGN KEY(`SessionId`) REFERENCES `Session`(`SessionId`) )",

		"CREATE TABLE `Sessions` ( `SessionId` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `Name` TEXT NOT NULL, `Place` TEXT, `Start` NUMERIC, `End` TEXT, `Note` TEXT, `Feeling` INTEGER )",

		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Weightlifting', 'Leimen', '2018-05-23 11:00', '2018-05-23 12:00')",
		
		"INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('Bodyweight', 'Sunnypark', '2018-05-24 09:00', '2018-05-23 09:30')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `SessionId`) VALUES ('Push-Ups', '2', '20', '2')",

		"INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `SessionId`) VALUES ('Squat', '5', '5', '80', '1')" };


	for (int i=0; i<6; i++)
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

static void sql_submit(char *filename, char *query)
{
	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, NULL))
		printf("error");
}

static int cmd_show_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	cb_called = 1;

	if (strcmp(argv[3], "0") == 0)
		printf("%s: %sx%s\n", argv[0], argv[1], argv[2]);
	else
		printf("%s: %skg %sx%s\n", argv[0], argv[3], argv[1], argv[2]);

	return 0;
}

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

void cmd_newlog(char *filename)
{
	open_db(filename);
	create_example_db(filename);
	close_db();
	exit(0);
}

void cmd_show(char *filename, char *session)
{
	char query[256];
	query[0] = '\0';

	sprintf(query, "SELECT Exercises.Name, Sets, Reps, Weight FROM Exercises INNER JOIN Sessions ON Exercises.SessionId=Sessions.SessionId WHERE Sessions.Name = '%s';", session);

    open_db(filename);

	char *szErrMsg = 0;
	int rc = sqlite3_exec(g_db, query, cmd_show_callback, 0, &szErrMsg);
	if( rc!=SQLITE_OK )
	{
		printf("SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}

	if (cb_called == 0)
		printf("No such session: %s\n", session);

	close_db();
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

	sql_submit(filename, query);

	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(b[i]);
	}

	close_db();
}

void cmd_list(char *filename)
{
	char *szErrMsg = 0;
	sqlite3_stmt *stmt = NULL;

	open_db(filename);

	int rc = sqlite3_prepare_v2(g_db, "SELECT Sessions.Name FROM Sessions", -1, &stmt, NULL );
	if( rc!=SQLITE_OK )
	{
		if (szErrMsg) {
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		} else {
			printf("SQL error: %s\n", szErrMsg);
		}
		close_db();
		exit(1);
	}

	printf("List of sessions:\n");
	printf("-----------------------\n");

	const char *data = NULL;
	while( sqlite3_step(stmt) == SQLITE_ROW ) {
		data = (const char*)sqlite3_column_text( stmt, 0 );
		printf( "%s\n", data ? data : "[NULL]" );
	}

	sqlite3_finalize(stmt);
	close_db();
}
