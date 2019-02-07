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

static void parse_config(char *path)
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
		CFG_END()
	};

	cfg_opt_t cfgopt[] = {
		CFG_SEC("insert-template", insert_template_opts, CFGF_MULTI | CFGF_TITLE),
		CFG_END()
	};

	cfg = cfg_init(cfgopt, CFGF_IGNORE_UNKNOWN);

	int ret = cfg_parse(cfg, path);

	if (ret == CFG_FILE_ERROR)
	{
		fprintf(stderr, "libconfuse: file error");
		// TODO: probably stop
	}
	else if (ret == CFG_PARSE_ERROR)
	{
		fprintf(stderr, "libconfuse: parse error");
		// TODO: probably stop
	}

	/* Debug:
	int n = cfg_size(cfg, "insert-template");
	printf("%d configured:\n", n);
	for (int i = 0; i < n; i++) {
		cfg_t *tmpl = cfg_getnsec(cfg, "insert-template", i);
		printf("template #%u (%s):\n", i + 1, cfg_title(tmpl));
		printf("se_omit_place = %s\n", cfg_getbool(tmpl, "se_omit_place") ? "true" : "false");
	}
	*/
}

void read_config()
{
	char *conf_path = choose_config();
	if (conf_path != NULL)
	{
		parse_config(conf_path);
		free(conf_path);
	}
}

void free_config()
{
	cfg_free(cfg);
}

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
