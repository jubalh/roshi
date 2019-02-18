/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <stdbool.h> // bool

#define CMDADD_OMIT_COUNT 13
enum CMDADD_OMITS {
	OM_SESSION_PLACE = 0,
	OM_SESSION_NOTES,
	OM_SESSION_FEELING,
	OM_EXERCISE_STATION,
	OM_EXERCISE_WEIGHT,
	OM_EXERCISE_SETS,
	OM_EXERCISE_REPS,
	OM_EXERCISE_TIME,
	OM_EXERCISE_TEMPO,
	OM_EXERCISE_REST,
	OM_EXERCISE_WARMUP,
	OM_EXERCISE_NOTES,
	OM_EXERCISE_TAGS,
	OM_EXERCISE_DISTANCE
};

int read_config();
void free_config();
bool fill_omit_vars(const char *template_name, bool *b);
