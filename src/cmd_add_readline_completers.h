/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

char ** cmd_add_completion_none(const char *text, int start, int end);
char ** cmd_add_completion_session_name(const char *text, int start, int end);
char ** cmd_add_completion_session_place(const char *text, int start, int end);
char ** cmd_add_completion_exercise_tag(const char *text, int start, int end);
char ** cmd_add_completion_exercise_name(const char *text, int start, int end);
