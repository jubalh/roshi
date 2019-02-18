/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <readline/readline.h> // rl_insert_text()
#include <time.h> // localtime(), strftime()

int readline_time_template_hook(void)
{
	time_t t;
	struct tm* tm_info;

	time(&t);
	tm_info = localtime(&t);

	char buffer[12];
	strftime(buffer, 12, "%H:%M", tm_info);

	return rl_insert_text(buffer);
}

int readline_date_template_hook(void)
{
	time_t t;
	struct tm* tm_info;

	time(&t);
	tm_info = localtime(&t);

	char buffer[12];
	strftime(buffer, 12, "%Y-%m-%d", tm_info);

	return rl_insert_text(buffer);
}

int readline_warmup_template_hook(void)
{
	return rl_insert_text("no");
}

int readline_feeling_template_hook(void)
{
	return rl_insert_text("0");
}
