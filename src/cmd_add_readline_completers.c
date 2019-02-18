/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <readline/readline.h> // rl_*()
#include "cmd_add_readline_generators.h" // *_generators()

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
