// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free() 
#include <unistd.h> // access()
#include <confuse.h> // cfg_*
#include "config.h" // roshi_config

#define CONF_FILE "roshi.conf"

void read_config(struct roshi_config *config)
{
	config->test = NULL;

	if (access(CONF_FILE, F_OK) >= 0)
	{
		cfg_opt_t cfgopt[] = {
			CFG_SIMPLE_STR("test", &config->test),
			CFG_END()
		};

		cfg_t *cfg;
		cfg = cfg_init(cfgopt, CFGF_IGNORE_UNKNOWN);
		cfg_parse(cfg, CONF_FILE);
		cfg_free(cfg);
	}
}

void free_config(struct roshi_config *config)
{
	free(config->test);
	config->test = NULL;
}
