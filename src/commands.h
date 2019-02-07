// vim: noexpandtab:ts=4:sts=4:sw=4

#pragma once

void cmd_newlog(const char *filename, int dummydata);
void cmd_show(const char *filename, const char *session, int isindex);
void cmd_add(const char* filename, const char* insert_template);
void cmd_list(const char *filename);
void cmd_pr(const char *filename);

void cmd_analyze();
void cmd_analyze_sub_pr(const char* filename);
void cmd_analyze_sub_test1(const char* filename);
