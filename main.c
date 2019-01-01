// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // strcmp()
#include <unistd.h> // access()
#include "commands.h"

static void print_usage()
{
	printf("Usage:\n");
	printf("roshi file_name newlog - Create new log\n");
	printf("roshi file_name list - List logged sessions\n");
	printf("roshi file_name show session_name - Display session\n");
	printf("roshi file_name add - Add session\n");
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		print_usage();
		exit(1);
	}

	if (strcmp(argv[2], "newlog") == 0)
	{
		cmd_newlog(argv[1]);
	} else {
		// if db doesnt exist or no further argument
		if ( (access(argv[1], F_OK) == -1) || argc < 3)
		{
			print_usage();
			exit(1);
		}

		if (strcmp(argv[2], "show") == 0)
		{
			if (argc < 4)
			{
				print_usage();
				exit(1);
			} else {
				cmd_show(argv[1], argv[3]);
			}
		} else if (strcmp(argv[2], "add") == 0) {
			cmd_add(argv[1]);
		} else if (strcmp(argv[2], "list") == 0) {
			cmd_list(argv[1]);
		} else {
			print_usage();
			exit(1);
		}
	}

	return 0;
}
