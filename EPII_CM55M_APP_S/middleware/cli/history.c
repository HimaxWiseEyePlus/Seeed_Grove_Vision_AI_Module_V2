/**************************************************************************************************
 (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
 ------------------------------------------------------------------------
 File        : uart_lib.c
 Project     : WEI
 DATE        : 2013/03/01
 AUTHOR      : 300212
 BRIFE       : This file implements history recorder
 HISTORY     : Initial version - 2013/03/01 created by will
 **************************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "history.h"

// Max number of history lines; must be power of 2
#define MAX_HISTORY 32

// Max number of edited history lines; must be power of 2
#define MAX_EDITING 4

#define hist_ptr(ptr, offset) ((ptr+offset)&(MAX_HISTORY-1))
#define edit_ptr(ptr, offset) ((ptr+offset)&(MAX_EDITING-1))

static char *history[MAX_HISTORY];
static int hist_head = 0, hist_tail = 0, browse = 0;

static char *EMPTY_STR = "";

typedef struct hist_edit_s {
	int idx;
	char *line;
} hist_edit_t;

static hist_edit_t editing[MAX_HISTORY];
static int edit_head = 0, edit_tail = 0;

static int find_editing(int idx);

void history_init(void) {
	/** init to default value **/
	memset(history, 0x00, MAX_HISTORY * sizeof(char*));
	memset(editing, 0x00, MAX_HISTORY * sizeof(hist_edit_t));
}

const char* history_previous(void) {
	int edit_idx;

	if (browse == hist_head)
		return NULL;

	browse = hist_ptr(browse, -1);

	edit_idx = find_editing(browse);
	if (edit_idx != -1)
		return editing[edit_idx].line;

	return history[browse];
}

const char* history_next(void) {
	int edit_idx;

	if (browse == hist_tail)
		return NULL;

	browse = hist_ptr(browse, 1);

	edit_idx = find_editing(browse);
	if (edit_idx != -1)
		return editing[edit_idx].line;

	return history[browse];
}

void history_add(const char *line) {
	char *buf;

	buf = (char*) malloc((strlen(line) + 1) * sizeof(char));
	if (buf == NULL)
		return;

	strcpy(buf, line);
	history[hist_tail] = buf;
	hist_tail = hist_ptr(hist_tail, 1);
	if (hist_tail == hist_head) {
		free(history[hist_head]);
		hist_head = hist_ptr(hist_head, 1);
	}

	history[hist_tail] = EMPTY_STR;
}

void history_edit(const char *line) {
	int edit_pos = find_editing(browse);
	char *buf;
	if (edit_pos != -1) {
		buf = (char*) realloc(editing[edit_pos].line,
				(strlen(line) + 1) * sizeof(char));
	} else {
		buf = (char*) malloc((strlen(line) + 1) * sizeof(char));

		edit_pos = edit_tail;
		editing[edit_pos].idx = browse;
		edit_tail = edit_ptr(edit_tail, 1);
		if (edit_tail == edit_head) {
			free(editing[edit_head].line);
			edit_head = edit_ptr(edit_head, 1);
		}

	}

	if (buf == NULL)
		return;

	strcpy(buf, line);
	editing[edit_pos].line = buf;
}

void history_forget_editing(void) {
	int i;
	for (i = edit_head; i != edit_tail; i = edit_ptr(i, 1)) {
		free(editing[i].line);
	}
	edit_head = edit_tail = 0;
	browse = hist_tail;
}

static int find_editing(int idx) {
	int i;
	for (i = edit_head; i != edit_tail; i = edit_ptr(i, 1)) {
		if (editing[i].idx == idx)
			return i;
	}

	return -1;
}
