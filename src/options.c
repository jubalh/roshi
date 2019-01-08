// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <string.h> // strcmp()
#include <stdio.h>
#include <unistd.h> // access(), getopt()
#include "commands.h"

int parse_options(int argc, char *argv[])
{
	//-----------------------
	// NEWLOG
    if (strcmp(argv[1], "newlog") == 0)
    {
		int opt;
		int dummydata = 0;

		while ((opt = getopt(argc, argv, "he")) != -1)
		{
			switch (opt)
			{
				case 'h':
					printf("newlog creates a new training log file.\n\n");
					printf("-e: Fill database with example values\n");
					printf("-h: Display this help\n");
					exit(EXIT_SUCCESS);
					break;
				case 'e':
					dummydata = 1;
					break;
				// invalid option
				default:
					return 2; // actually EXIT_FAILURE, but we don't want usage to be displayed
			}
		}

        cmd_newlog(argv[0], dummydata);
    } else {
        // if db doesnt exist or no further argument
        if ( (access(argv[0], F_OK) == -1) || argc < 3)
        {
            return EXIT_FAILURE;
        }

		//-----------------------
		// SHOW
        if (strcmp(argv[1], "show") == 0)
        {
            if (argc < 4)
            {
                return EXIT_FAILURE;
            } else {
                cmd_show(argv[0], argv[2]);
            }
		//-----------------------
		// ADD
        } else if (strcmp(argv[1], "add") == 0) {
            cmd_add(argv[0]);
		//-----------------------
		// LIST
        } else if (strcmp(argv[1], "list") == 0) {
            cmd_list(argv[0]);
		//-----------------------
		// ANALYZE
        } else if (strcmp(argv[1], "analyze") == 0) {
            cmd_analyze(argv[0]);
		//-----------------------
		// PR
        } else if (strcmp(argv[1], "pr") == 0) {
            cmd_pr(argv[0]);
        } else {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
