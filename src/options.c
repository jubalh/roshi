// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <string.h> // strcmp()
#include <stdio.h> // printf()
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
				printf("roshi \033[3mfile_name\033[0m newlog -- create new workoutlog\n\n");
				printf("-e: Fill workoutlog with example data\n");
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
	return EXIT_SUCCESS;
}

static void display_help_show()
{
	printf("roshi \033[3mfile_name\033[0m show \033[3msession_name\033[0m -- display a session\n\n");
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
	} else {
		cmd_show(argv[0], session_search, 1);
	}

	free(session_search);

	return EXIT_SUCCESS;
}

static int parse_add(int argc, char *argv[])
{
	int opt;
	char* insert_template = NULL;

	if (argc >= 3)
	{
		insert_template = argv[2];
	}

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt)
		{
			// help
			case 'h':
				printf("roshi \033[3mfile_name\033[0m add [insert-template] -- add a session to workoutlog\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_add(argv[0], insert_template);
	return EXIT_SUCCESS;
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
				printf("roshi \033[3mfile_name\033[0m list -- list all logged sessions\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_list(argv[0]);
	return EXIT_SUCCESS;
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
				printf("roshi \033[3mfile_name\033[0m analyze -- special analyzation functions\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	if (argc >= 3)
	{
		if (strcmp(argv[2], "pr") == 0)
		{
			cmd_analyze_sub_pr(argv[0]);
			return EXIT_SUCCESS;
		} else if (strcmp(argv[2], "test1") == 0)
		{
			cmd_analyze_sub_test1(argv[0]);
			return EXIT_SUCCESS;
		}
	}
	cmd_analyze();
	return EXIT_SUCCESS;
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
				printf("roshi \033[3mfile_name\033[0m pr -- list all personal records\n\n");
				printf("-h: Display this help\n");
				exit(EXIT_SUCCESS);
				break;
			// invalid option
			default:
				return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
		}
	}

	cmd_pr(argv[0]);
	return EXIT_SUCCESS;
}

int parse_options(int argc, char *argv[])
{
	if (strcmp(argv[1], "newlog") == 0)
	{
		return parse_newlog(argc, argv);
	} else {
		// if db doesnt exist or no further argument
		if ( (access(argv[0], F_OK) == -1) || argc < 2)
		{
			fprintf(stderr, "Workoutlog '%s' does not exist\n", argv[0]);
			return EXIT_FAILURE;
		}

		if (strcmp(argv[1], "show") == 0)
		{
			return parse_show(argc, argv);
		} else if (strcmp(argv[1], "add") == 0) {
			return parse_add(argc, argv);
		} else if (strcmp(argv[1], "list") == 0) {
			return parse_list(argc, argv);
		} else if (strcmp(argv[1], "analyze") == 0) {
			return parse_analyze(argc, argv);
		} else if (strcmp(argv[1], "pr") == 0) {
			return parse_pr(argc, argv);
		} else {
			fprintf(stderr, "No such subcommand\n");
			return EXIT_FAILURE;
		}
	}
}
