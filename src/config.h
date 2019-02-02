// vim: noexpandtab:ts=4:sts=4:sw=4

#pragma once

struct roshi_config {
	char *test;
};

void read_config(struct roshi_config *config);
void free_config(struct roshi_config *config);
