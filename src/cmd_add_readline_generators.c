// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h> // strdup()
#include <string.h> // snprintf()
#include "sql.h" // sqlite3_*

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
