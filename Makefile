# vim: noexpandtab:ts=4:sts=4:sw=4

roshi: main.c clean
	gcc -g -o roshi main.c sql.c commands.c cmd_add.c -lsqlite3 -lreadline

clean:
	if [ -e roshi ]; then \
		rm roshi || exit 1; \
	fi
