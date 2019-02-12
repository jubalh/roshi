// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free()
#include <time.h> // localtime(), strftime()
#include <stdio.h>  // printf()
#include <string.h> // strtok(), strcmp()
#include <readline/readline.h> // rl_*()
#include "sql.h" // open_db()
#include "cmd_add_readline_completers.h" // cmd_add_completion_*()
#include "cmd_add_readline_hooks.h" // readline_*_template_hook()
#include "config.h" // fill_omit_vars()

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

// Nr of EXERCISE_FIELDS
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

static bool validate(char *str, unsigned int flag, unsigned int max_length)
{
	// dont allow some SQL chars
	if ( (strchr(str, '\'') > 0) || (strchr(str, '"') > 0) || (strchr(str, ';') > 0) )
	{
		return false;
	}
	// check for max length
	if ((max_length != 0) && (strlen(str) > max_length))
	{
		return false;
	}

	return true;
}

static void read_valid(char **dst, char *prompt, unsigned int max_length)
{
	bool valid = false;
	*dst = readline(prompt);

	valid = validate(*dst, 0, max_length);
	while (!valid)
	{
		printf("Invalid input\n");

		free(*dst);
		*dst = readline(prompt);
		valid = validate(*dst, 0, max_length);
	}
}

static sqlite3_int64 collect_submit_session(char **bs, bool* omit)
{
	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
		bs[i] = NULL;

	rl_attempted_completion_function = cmd_add_completion_session_name;

	// collect session info
	read_valid(&bs[SNAME], "Session name: ", 0);

	if (omit[OM_SESSION_PLACE] == false)
	{
		rl_attempted_completion_function = cmd_add_completion_session_place;
		read_valid(&bs[SPLACE], "Place: ", 0);
		rl_attempted_completion_function = cmd_add_completion_none;
	}

	rl_startup_hook = readline_date_template_hook;
	read_valid(&bs[SDATE], "Date: ", 10);
	rl_startup_hook = readline_time_template_hook;
	read_valid(&bs[SSTART], "Start time: ", 5);
	read_valid(&bs[SEND], "End time: ", 5);
	rl_startup_hook = NULL;

	if (omit[OM_SESSION_NOTES] == false)
	{
		//TODO: create big enough buffer here and for query. since notes could be big.
		read_valid(&bs[SNOTES], "Notes: ", 0);
	}

	if (omit[OM_SESSION_FEELING] == false)
	{
		rl_startup_hook = readline_feeling_template_hook;
		read_valid(&bs[SFEELING], "Feeling (-2 to 2): ", 2);
		rl_startup_hook = NULL;
	}

	// submit session
	char query[4096];
	snprintf(query, 4095, "INSERT INTO `Sessions` (`Name`, `Place`, `Start`, `End`) VALUES ('%s', '%s', '%s %s', '%s %s')", bs[SNAME], bs[SPLACE], bs[SDATE], bs[SSTART], bs[SDATE], bs[SEND]);

	if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, NULL))
		printf("error");

	// get session id
	sqlite3_int64 session_id = sqlite3_last_insert_rowid(g_db);

	// cleanup
	for (int i=0; i<MAX_S_ENUM_FIELDS; i++)
	{
		free(bs[i]);
	}

	return session_id;
}

static void collect_submit_exercises(char **be, sqlite3_int64 session_id, bool *omit)
{
	// init exercise buffers
	for (int i=0; i<MAX_E_ENUM_FIELDS; i++)
		be[i] = NULL;

	// collect exercise info
	printf("\nExercises:\n");

	rl_attempted_completion_function = cmd_add_completion_exercise_name;
	read_valid(&be[EXNAME], "Exercise name: ", 0);
	rl_attempted_completion_function = cmd_add_completion_none;

	char query[4096];
	while (strcmp(be[EXNAME], "") != 0)
	{
		if (omit[OM_EXERCISE_STATION] == false)
		{
			read_valid(&be[EXSTATION], "Station: ", 5);
		}
		if (omit[OM_EXERCISE_WEIGHT] == false)
		{
			read_valid(&be[EXWEIGHT], "Weight (kg): ", 0);
		}
		if (omit[OM_EXERCISE_SETS] == false)
		{
			read_valid(&be[EXSETS], "Sets: ", 5);
		}
		if (omit[OM_EXERCISE_REPS] == false)
		{
			read_valid(&be[EXREPS], "Reps: ", 5);
		}
		if (omit[OM_EXERCISE_TIME] == false)
		{
			read_valid(&be[EXTIME], "Time/Duration (min): ", 0);
		}
		if (omit[OM_EXERCISE_TEMPO] == false)
		{
			read_valid(&be[EXTEMPO], "Tempo: ", 4);
		}
		if (omit[OM_EXERCISE_REST] == false)
		{
			read_valid(&be[EXREST], "Rest time (min): ", 0);
		}
		if (omit[OM_EXERCISE_WARMUP] == false)
		{
			rl_startup_hook = readline_warmup_template_hook;
			read_valid(&be[EXISWARMUP], "Warmup set: ", 3);
			rl_startup_hook = NULL;
		}
		if (omit[OM_EXERCISE_NOTES] == false)
		{
			read_valid(&be[EXNOTE], "Notes: ", 0);
		}
		if (omit[OM_EXERCISE_TAGS] == false)
		{
			rl_attempted_completion_function = cmd_add_completion_exercise_tag;
			read_valid(&be[EXTAGS], "Tags: ", 0);
			rl_attempted_completion_function = cmd_add_completion_none;
		}

		// transform some of the inputs so they fit into db
		int t_isWarmup = 0;
		if (omit[OM_EXERCISE_WARMUP] == false)
		{
			if (strcmp(be[EXISWARMUP], "yes") == 0)
			{
				t_isWarmup = 1;
			}
		}

		long int t_time = 0;
		long int t_rest = 0;
		// TODO: might support 's' 'm' 'h' after the digit
		// user enters in minutes (for now); save in db as seconds
		if (omit[OM_EXERCISE_REST] == false)
		{
			t_rest = strtol(be[EXREST], NULL, 10) * 60;
		}
		if (omit[OM_EXERCISE_TIME] == false)
		{
			t_time = strtol(be[EXTIME], NULL, 10) * 60;
		}

		// submit exercise
		char *szErrMsg = 0;
		snprintf(query, 4095, "INSERT INTO `Exercises` (`Name`, `Sets`, `Reps`, `Weight`, `Time`, `Tempo`, `Rest`, `isWarmup`, `Note`, `Station`, `SessionID`) VALUES ('%s', '%s', '%s', '%s', '%ld', '%s', '%ld', '%d', '%s', '%s', '%d')", be[EXNAME], be[EXSETS], be[EXREPS], be[EXWEIGHT], t_time, be[EXTEMPO], t_rest, t_isWarmup, be[EXNOTE], be[EXSTATION], session_id);
		if(SQLITE_OK != sqlite3_exec(g_db, query, NULL, 0, &szErrMsg))
		{
			printf("SQL error: %s\n", szErrMsg);
			sqlite3_free(szErrMsg);
		}

		sqlite3_int64 exercise_id = sqlite3_last_insert_rowid(g_db);

		if (omit[OM_EXERCISE_TAGS] == false)
		{
			sqlite3_stmt *stmt_tag= NULL;
			sqlite3_stmt *stmt_tag_ex = NULL;

			sqlite3_prepare_v2(g_db, "INSERT INTO Tags (Name) SELECT :strtagname WHERE NOT EXISTS (SELECT TagId FROM Tags WHERE Name = :strtagname)", -1, &stmt_tag, NULL );

			//TODO: only insert once. dont do if exercise already has link to tag
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
		}

		for (int i=0; i<MAX_E_ENUM_FIELDS; i++)
		{
			free(be[i]);
			be[i] = NULL;
		}

		rl_attempted_completion_function = cmd_add_completion_exercise_name;
		read_valid(&be[EXNAME], "\nExercise name: ", 0);
		rl_attempted_completion_function = cmd_add_completion_none;
	}

	if (be[EXNAME] != NULL)
	{
		free(be[EXNAME]);
	}
}

void cmd_add(const char* filename, const char* insert_template)
{
	bool omit[CMDADD_OMIT_COUNT];
	fill_omit_vars(insert_template, omit);

	open_db(filename);

	printf("New Session\n");
	printf("-----------------------\n");

	char *bs[MAX_S_ENUM_FIELDS];
	sqlite3_int64 session_id = collect_submit_session(bs, omit);

	char *be[MAX_E_ENUM_FIELDS];
	collect_submit_exercises(be, session_id, omit);

	close_db();
}
