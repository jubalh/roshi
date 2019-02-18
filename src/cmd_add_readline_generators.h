/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

char * generic_generator(const char *table, const char *text, int state, const char *col);
char * session_name_generator(const char *text, int state);
char * session_place_generator(const char *text, int state);
char * exercise_tag_generator(const char *text, int state);
char * exercise_name_generator(const char *text, int state);
