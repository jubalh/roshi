// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h> // printf()
#include <stdlib.h> // EXIT_SUCCESS
#include "options.h"

#define VERSION "0.0"

static void print_usage()
{

#ifndef GIT_REV
	printf("roshi v"VERSION"\n\n");
#else 
	printf("roshi v"VERSION"~"GIT_REV"\n\n");
#endif

	printf("Usage:\n");
	printf("roshi \e[3mfile_name\e[0m newlog - Create new training log\n");
	printf("roshi \e[3mfile_name\e[0m list - List logged training sessions\n");
	printf("roshi \e[3mfile_name\e[0m show session_name - Display training session\n");
	printf("roshi \e[3mfile_name\e[0m add - Add trainig session\n");
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
