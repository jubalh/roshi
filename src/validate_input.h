/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

enum INPUT_VALIDATE_ENUM {
	INP_VALIDATE_DATE = 1,
	INP_VALIDATE_TIME = 2,
};

bool validate(char *str, unsigned int flag, unsigned int max_length);
