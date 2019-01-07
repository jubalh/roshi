// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // exit()
#include <string.h> // strcmp()
#include <unistd.h> // access()
#include "commands.h"

int parse_options(int argc, char *argv[])
{
    if (strcmp(argv[2], "newlog") == 0)
    {
        cmd_newlog(argv[1]);
    } else {
        // if db doesnt exist or no further argument
        if ( (access(argv[1], F_OK) == -1) || argc < 3)
        {
            return EXIT_FAILURE;
        }

        if (strcmp(argv[2], "show") == 0)
        {
            if (argc < 4)
            {
                return EXIT_FAILURE;
            } else {
                cmd_show(argv[1], argv[3]);
            }
        } else if (strcmp(argv[2], "add") == 0) {
            cmd_add(argv[1]);
        } else if (strcmp(argv[2], "list") == 0) {
            cmd_list(argv[1]);
        } else if (strcmp(argv[2], "analyze") == 0) {
            cmd_analyze(argv[1]);
        } else if (strcmp(argv[2], "pr") == 0) {
            cmd_pr(argv[1]);
        } else {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
