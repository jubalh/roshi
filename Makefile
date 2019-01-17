# vim: noexpandtab:ts=4:sts=4:sw=4

src = src/main.c src/sql.c src/commands.c src/cmd_newlog.c src/cmd_add.c src/options.c src/cmd_analyze.c

STR_SUCCESS = \e[1;32mSUCCESS\e[0m
STR_FAIL = \e[1;31mFAIL\e[0m

roshi: $(src) clean
	if [ -e .git/HEAD ]; then \
		gcc -g -o roshi $(src) -lsqlite3 -lreadline -DGIT_REV='"$(shell git log --pretty=format:'%h' -n 1)"'; \
	else \
		gcc -g -o roshi $(src) -lsqlite3 -lreadline; \
	fi

clean:
	if [ -e roshi ]; then \
		rm roshi || exit 1; \
	fi

test: roshi
	if [ -e maketest.db ]; then \
		rm maketest.db || exit 1; \
	fi

	@echo -e "Testing:"
	@./roshi maketest.db newlog -e
	@sqlite3 -line maketest.db 'select * from Exercises;' > sql1.is

ifneq ($(shell diff -q tests/expected_result_1 sql1.is 1>&2 2> /dev/null; echo $$?) ,0)
	@echo -e "roshi newlog $(STR_FAIL)"
else
	@echo -e "roshi newlog $(STR_SUCCESS)"
endif
	rm sql1.is maketest.db
