// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free() 
#include <stdbool.h> // bool
#include <string.h> // strdup()
#include <unistd.h> // access()
#include <confuse.h> // cfg_*
#include "config.h" // roshi_config

#define CONF_FILE "roshi.conf"

static char* conf_path = NULL;

static bool set_conf_path(char *fn)
{
	if (access(fn, F_OK) >= 0)
	{
		conf_path = strdup(fn);
		return true;
	}
	return false;
}

static bool choose_config(struct roshi_config *config)
{
	if (set_conf_path(CONF_FILE))
		return true;

	char tmp_path[4096];
	char *p = getenv("XDG_CONFIG_HOME");
	if (p != NULL)
	{
		sprintf(tmp_path, "%s/roshi/config", p);
		if (set_conf_path(tmp_path))
			return true;
	}

	p = getenv("HOME");
	if (p != NULL)
	{
		sprintf(tmp_path, "%s/.config/roshi/config", p);
		if (set_conf_path(tmp_path))
			return true;
	}

	return false;
}

void read_config(struct roshi_config *config)
{
	config->test = NULL;

	if (choose_config(config))
	{
		cfg_opt_t cfgopt[] = {
			CFG_SIMPLE_STR("test", &config->test),
			CFG_END()
		};

		cfg_t *cfg;
		cfg = cfg_init(cfgopt, CFGF_IGNORE_UNKNOWN);
		cfg_parse(cfg, conf_path);
		cfg_free(cfg);
		free(conf_path);
	}
}

void free_config(struct roshi_config *config)
{
	free(config->test);
	config->test = NULL;
}
