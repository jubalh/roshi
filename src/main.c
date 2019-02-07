// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h> // printf()
#include <stdlib.h> // EXIT_SUCCESS
#include "options.h" // parse_options()
#include "config.h" // read_config()

#define VERSION "0.0"

static void print_usage()
{

#ifndef GIT_REV
	printf("roshi v"VERSION"\n\n");
#else 
	printf("roshi v"VERSION"~"GIT_REV"\n\n");
#endif

	printf("Usage:\n");
	printf("roshi \033[3mfile_name\033[0m newlog - Create new training log\n");
	printf("roshi \033[3mfile_name\033[0m list - List logged training sessions\n");
	printf("roshi \033[3mfile_name\033[0m show session_name - Display training session\n");
	printf("roshi \033[3mfile_name\033[0m add - Add trainig session\n");
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		print_usage();
		return EXIT_SUCCESS;
	}

	if (read_config() == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	int ret = parse_options(argc-1, argv+1);
	free_config();

	return ret;
}
