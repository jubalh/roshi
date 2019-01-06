# vim: noexpandtab:ts=4:sts=4:sw=4

roshi: src/main.c clean
	if [ -e .git/HEAD ]; then \
		gcc -g -o roshi src/main.c src/sql.c src/commands.c src/cmd_newlog.c src/cmd_add.c src/options.c -lsqlite3 -lreadline -DGIT_REV='"$(shell git log --pretty=format:'%h' -n 1)"'; \
	else \
		gcc -g -o roshi src/main.c src/sql.c src/commands.c src/cmd_newlog.c src/cmd_add.c src/options.c -lsqlite3 -lreadline; \
	fi

clean:
	if [ -e roshi ]; then \
		rm roshi || exit 1; \
	fi
