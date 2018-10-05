# vim: noexpandtab:ts=4:sts=4:sw=4

roshi: main.c clean
	gcc -g -o roshi main.c commands.c -lsqlite3 -lreadline

clean:
	rm roshi
