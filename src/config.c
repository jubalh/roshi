// vim: noexpandtab:ts=4:sts=4:sw=4

#include <stdlib.h> // free() 
#include <stdbool.h> // bool
#include <string.h> // strdup()
#include <unistd.h> // access()
#include <confuse.h> // cfg_*
#include "config.h" // enum CMDADD_OMITS

#define CONF_FILE "roshi.conf"

static cfg_t *cfg = NULL;

// checks whether config is readable at fn
// returns full path if true. NULL if not.
static char* set_conf_path(char *fn)
{
	if (access(fn, F_OK) >= 0)
	{
		return strdup(fn);
	}
	return NULL;
}

// chooses config after following prio:
// 1. local, 2. XDG_CONFIG_HOME 3. ~/.config
// returns the path that got hit first
static char* choose_config()
{
	char *path;
	path = set_conf_path(CONF_FILE);
	if (path != NULL)
		return path;

	char tmp_path[4096];
	char *p = getenv("XDG_CONFIG_HOME");
	if (p != NULL)
	{
		sprintf(tmp_path, "%s/roshi/config", p);
		path = set_conf_path(tmp_path);
		if (path != NULL)
			return path;
	}

	p = getenv("HOME");
	if (p != NULL)
	{
		sprintf(tmp_path, "%s/.config/roshi/config", p);
		path = set_conf_path(tmp_path);
		if (path != NULL)
			return path;
	}

	return NULL;
}

static int parse_config(char *path)
{
	cfg_opt_t insert_template_opts[] = {
		CFG_BOOL("omit_session_place", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_session_notes", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_session_feeling", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_station", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_weight", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_sets", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_reps", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_time", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_tempo", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_rest", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_warmup", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_notes", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_tags", cfg_false, CFGF_NONE),
		CFG_BOOL("omit_exercise_distance", cfg_false, CFGF_NONE),
		CFG_END()
	};

	cfg_opt_t cfgopt[] = {
		CFG_SEC("insert-template", insert_template_opts, CFGF_MULTI | CFGF_TITLE),
		CFG_END()
	};

	//for now lets warn, so that the user finds typos
	//cfg = cfg_init(cfgopt, CFGF_IGNORE_UNKNOWN);
	cfg = cfg_init(cfgopt, 0);

	int ret = cfg_parse(cfg, path);

	if (ret == CFG_FILE_ERROR)
	{
		fprintf(stderr, "libconfuse: file error\n");
		return EXIT_FAILURE;
	}
	else if (ret == CFG_PARSE_ERROR)
	{
		fprintf(stderr, "Error in configuration file %s\n", path);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// returns:
// EXIT_SUCCESS (0) if config read succesfully
// EXIT_FAILURE (1) if there was a parse or other error
// 3 if there was no config found
int read_config()
{
	int ret = EXIT_SUCCESS;
	char *conf_path = choose_config();

	if (conf_path != NULL)
	{
		ret = parse_config(conf_path);
		free(conf_path);
	} else {
		ret = 3;
	}

	return ret;
}

void free_config()
{
	cfg_free(cfg);
}

// returns:
// true if variables were set from config
// false if all were just set to false
bool fill_omit_vars(const char *template_name, bool *b)
{
	if ((cfg != NULL) && (template_name != NULL))
	{
		int n = cfg_size(cfg, "insert-template");
		for (int i = 0; i < n; i++)
		{
			cfg_t *tmpl = cfg_getnsec(cfg, "insert-template", i);
			if (strcmp(template_name, cfg_title(tmpl)) == 0)
			{
				b[OM_SESSION_PLACE] = cfg_getbool(tmpl, "omit_session_place");
				b[OM_SESSION_NOTES] = cfg_getbool(tmpl, "omit_session_notes");
				b[OM_SESSION_FEELING] = cfg_getbool(tmpl, "omit_session_feeling");
				b[OM_EXERCISE_STATION] = cfg_getbool(tmpl, "omit_exercise_station");
				b[OM_EXERCISE_WEIGHT] = cfg_getbool(tmpl, "omit_exercise_weight");
				b[OM_EXERCISE_SETS] = cfg_getbool(tmpl, "omit_exercise_sets");
				b[OM_EXERCISE_REPS] = cfg_getbool(tmpl, "omit_exercise_reps");
				b[OM_EXERCISE_TIME] = cfg_getbool(tmpl, "omit_exercise_time");
				b[OM_EXERCISE_TEMPO] = cfg_getbool(tmpl, "omit_exercise_tempo");
				b[OM_EXERCISE_REST] = cfg_getbool(tmpl, "omit_exercise_rest");
				b[OM_EXERCISE_WARMUP] = cfg_getbool(tmpl, "omit_exercise_warmup");
				b[OM_EXERCISE_NOTES] = cfg_getbool(tmpl, "omit_exercise_notes");
				b[OM_EXERCISE_TAGS] = cfg_getbool(tmpl, "omit_exercise_tags");
				b[OM_EXERCISE_DISTANCE] = cfg_getbool(tmpl, "omit_exercise_distance");
				return true;
			}
		}
	}

	for (int i = 0; i < CMDADD_OMIT_COUNT; i++)
	{
		b[i] = false;
	}

	return false;
}
