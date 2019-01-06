// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h> // printf()
#include <stdlib.h> // EXIT_SUCCESS
#include "options.h"

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
	if (argc < 3 || EXIT_FAILURE == parse_options(argc, argv))
	{
		print_usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
