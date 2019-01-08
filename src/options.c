// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <string.h> // strcmp()
#include <stdio.h>
#include <unistd.h> // access(), getopt()
#include "commands.h"

static int parse_newlog(int argc, char *argv[])
{
	int opt;
	int dummydata = 0;

	while ((opt = getopt(argc, argv, "he")) != -1)
	{
		switch (opt)
		{
			// help
			case 'h':
				printf("roshi \e[3mfile_name\e[0m newlog: creates a new training log file.\n\n");
				printf("-e: Fill database with example values\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// example data
			case 'e':
				dummydata = 1;
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	if (access(argv[0], F_OK) >= 0)
	{
		printf("Aborting: File already exists.\n");
		return 2;
	}

	cmd_newlog(argv[0], dummydata);
}

static void display_help_show()
{
	printf("roshi \e[3mfile_name\e[0m show \e[3msession_name\e[0m: displays a session.\n\n");
	printf("-i: Lookup session by ID instead of name\n");
	printf("-h: Display this help\n");
}

static int parse_show(int argc, char *argv[])
{
	int opt;
	char *session_search = NULL;

	while ((opt = getopt(argc, argv, "hi:")) != -1)
	{
		switch (opt)
		{
			// search by index
			case 'i':
				session_search = strdup(optarg);
				break;
			// help
			case 'h':
				display_help_show();
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	if (session_search == NULL)
	{
		if (argc < 3)
		{
			display_help_show();
			return 2;
		}

		session_search = strdup(argv[2]);
		cmd_show(argv[0], session_search, 0);
		free(session_search);
	} else {
		cmd_show(argv[0], session_search, 1);
	}
}

static int parse_add(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt)
		{
			// help
			case 'h':
				printf("roshi \e[3mfile_name\e[0m add: adds a session.\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_add(argv[0]);
}

static int parse_list(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt)
		{
			// help
			case 'h':
				printf("roshi \e[3mfile_name\e[0m list: lists all logged sessions.\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_list(argv[0]);
}

static int parse_analyze(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt)
		{
			// help
			case 'h':
				printf("roshi \e[3mfile_name\e[0m analyze special analyzation functions.\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_analyze(argv[0]);
}

static int parse_pr(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt)
		{
			// help
			case 'h':
				printf("roshi \e[3mfile_name\e[0m pr: lists all personal records.\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_pr(argv[0]);
}

int parse_options(int argc, char *argv[])
{
	if (strcmp(argv[1], "newlog") == 0)
	{
		parse_newlog(argc, argv);
	} else {
		// if db doesnt exist or no further argument
		if ( (access(argv[0], F_OK) == -1) || argc < 2)
		{
			return EXIT_FAILURE;
		}

		if (strcmp(argv[1], "show") == 0)
		{
			parse_show(argc, argv);
		} else if (strcmp(argv[1], "add") == 0) {
			parse_add(argc, argv);
		} else if (strcmp(argv[1], "list") == 0) {
			parse_list(argc, argv);
		} else if (strcmp(argv[1], "analyze") == 0) {
			parse_analyze(argc, argv);
		} else if (strcmp(argv[1], "pr") == 0) {
			parse_pr(argc, argv);
		} else {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
