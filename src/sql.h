/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <sqlite3.h>

extern sqlite3 *g_db;

void open_db(const char *filename);
void close_db();
