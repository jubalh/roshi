/*
 * vim: noexpandtab:ts=4:sts=4:sw=4
 *
 * Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <string.h> // strchr()
#include <stdbool.h> // bool
#include <ctype.h> // isdigit()
#include "validate_input.h" // INPUT_VALIDATE_ENUM

bool validate(char *str, unsigned int flag, unsigned int max_length)
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

	if (flag == INP_VALIDATE_DATE)
	{
		if (strlen(str) != 10 ||
				str[4] != '-' ||
				str[7] != '-')
		{
			return false;
		}
		for (int i = 0; i < strlen(str); i++)
		{
			if (isdigit(str[i]) == 0)
			{
				if ((i == 4) || (i == 7))
					continue;
				return false;
			}
		}
	}
	else if (flag == INP_VALIDATE_TIME)
	{
		if (strlen(str) != 5 ||
				str[2] != ':' ||
				isdigit(str[0]) == 0 ||
				isdigit(str[1]) == 0 ||
				isdigit(str[3]) == 0 ||
				isdigit(str[4]) == 0)
		{
			return false;
		}
	}

	return true;
}
