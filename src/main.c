// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdio.h> // printf()
#include <stdlib.h> // EXIT_SUCCESS
#include <confuse.h> // cfg_*
#include "options.h" // parse_options()
#include "config.h" // roshi_config

#define VERSION "0.0"
#define CONF_FILE "roshi.conf"

static void read_config(struct roshi_config *config)
{
	config->test = NULL;

	cfg_opt_t cfgopt[] = {
		CFG_SIMPLE_STR("test", &config->test),
		CFG_END()
	};

	cfg_t *cfg;
	cfg = cfg_init(cfgopt, CFGF_IGNORE_UNKNOWN);
	cfg_parse(cfg, CONF_FILE);
	cfg_free(cfg);
}

static void free_config(struct roshi_config *config)
{
	free(config->test);
	config->test = NULL;
}

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
	struct roshi_config config;
	read_config(&config);

	if (argc < 3)
	{
		print_usage();
		return EXIT_SUCCESS;
	}

	int ret = parse_options(argc-1, argv+1);
	free_config(&config);

	return ret;
}
