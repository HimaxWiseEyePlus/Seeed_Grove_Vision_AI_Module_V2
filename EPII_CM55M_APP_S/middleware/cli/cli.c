/**************************************************************************************************
 (C) COPYRIGHT, Himax Technologies Solution, Inc. ALL RIGHTS RESERVED
 ------------------------------------------------------------------------
 File        : cli.c
 Project     : WEI
 DATE        : 2013/03/01
 AUTHOR      : 300212
 BRIFE       : This file implements the CLI Library.
 HISTORY     : Initial version - 2013/03/01 created by 300212
 2013/07/18 update by 300212 - Fix Issue : heap full error
 **************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cli.h"
#include "cli_debug.h"
#include "cli_console.h"
#include "cli_cmd.h"

// HMX@WILL - define feature "CLI_ENABLE_PASSWORD" in
#ifdef CLI_ENABLE_PASSWORD
#include "passwords.txt"
#endif

#include "history.h"
#include "WE2_device.h"

#define MAX_PATH_DEPTH          10
#define MAX_BATCH_LEVEL         10
#define BATCH_THREAD_STACK_SIZE 0

#define ADD_CMD(name, handler_func, desc)   ADD_CMD_SECRET(name, handler_func, desc, 0)
#define ADD_MENU(name, menu_name, desc)     ADD_MENU_SECRET(name, menu_name, desc, 0)

#define CLI_MENU_ENUM(name)     CLI_MENU_ENUM_##name
#define CLI_MENU(name)          (cli_menus[CLI_MENU_ENUM(name)])
#define CLI_MENU_ITEMS(name)    (cli_menu_items_##name)

typedef enum {
#define BEGIN_MENU(name) CLI_MENU_ENUM(name), 
#define END_MENU
#define ADD_CMD_SECRET(name, handler_func, desc, level)
#define ADD_MENU_SECRET(name, menu_name, desc, level)
#define ADD_LINK(name, menu, item, level) 
#include "../app/integrate/cli_global/menu.h"
#include "cli_builtin_menu.h"
#undef BEGIN_MENU
#undef END_MENU
#undef ADD_CMD_SECRET
#undef ADD_MENU_SECRET
#undef ADD_LINK
	CLI_MENU_ID_MAX
} CLI_MENU_ENUM_T;

struct CLI_MENU_S;

typedef struct CLI_ITEM_S {
	const char *name;
	const char *desc;
	enum {
		CMD, MENU, LINK
	} type;
	int secret_level;

	CLI_CMD_HANDLER_T handler;
	UINT32_T menu; // value of CLI_MENU_ENUM_T for menu or (item_offset<<16)|menu for link

} CLI_ITEM_T;

typedef struct CLI_MENU_S {
	const char *name;
	CLI_ITEM_T *items;
} CLI_MENU_T;

typedef struct CLI_BATCH_S CLI_BATCH_T;

typedef struct {
	CLI_ITEM_T *cmd_item;
	int argc;
	char **argv;
} CLI_BATCHED_CMD_T;

typedef struct CLI_BATCH_ITEM_S {
	enum {
		SINGLE, BATCH
	} type;
	union {
		CLI_BATCHED_CMD_T *cmd;
		CLI_BATCH_T *batch;
	} item;

	struct CLI_BATCH_ITEM_S *next;
} CLI_BATCH_ITEM_T;

struct CLI_BATCH_S {
	enum {
		SEQUENTIAL = 0, RANDOM = 1, PARALLEL = 2, RANDOM_ONE = 4
	} exe_manner;
	int loop_count;
	int item_count;
	CLI_BATCH_ITEM_T *items_head;
	CLI_BATCH_ITEM_T *items_tail;
};

typedef struct result_list_s {
	char *value;
	struct result_list_s *next;
} result_list_t;

// macros for menu-struct name, menu-struct field name, and item offset within menu-struct;
// menu-struct is for item linkage, and offset is for referencing linkage target item
#define CLI_MENU_STRUCT(name)       __CLI_STRUCT_OF_MENU__##name
#define CLI_MENU_STRUCT_FIELD(name) __CLI_MENU_STRUCT_FIELD_OF_ITEM__##name
#define CLI_ITEM_OFFSET(menu, item) ((int)(&((struct CLI_MENU_STRUCT(menu)*)0)->CLI_MENU_STRUCT_FIELD(item))>>2)
#define CLI_LINK(menu, item)        (((((UINT32_T)CLI_ITEM_OFFSET(menu, item))<<16)&0xFFFF0000)|(((UINT32_T)CLI_MENU_ENUM_##menu)&0xFFFF))

/**************************************************************/
/* built-in commands                                          */
/**************************************************************/
static BOOL_T cli_builtin_help(int argc, char *argv[]);
static BOOL_T cli_builtin_cd(int argc, char *argv[]);
static BOOL_T cli_builtin_ls(int argc, char *argv[]);
static BOOL_T cli_builtin_pwd(int argc, char *argv[]);
static BOOL_T cli_builtin_begin(int argc, char *argv[]);
static BOOL_T cli_builtin_end(int argc, char *argv[]);
static BOOL_T cli_builtin_cancel(int argc, char *argv[]);
static BOOL_T cli_builtin_load(int argc, char *argv[]);
static BOOL_T cli_builtin_level(int argc, char *argv[]);

/**************************************************************/
/* Define command handlers by ADD_CMD_SECRET for each Module  */
/**************************************************************/
#define BEGIN_MENU(name)
#define END_MENU
#define ADD_CMD_SECRET(name, handler_func, desc, level) extern BOOL_T handler_func(int argc, char* args[]);
#define ADD_MENU_SECRET(name, menu_name, desc, level)
#define ADD_LINK(name, menu, item, level) 
#include "../app/integrate/cli_global/menu.h"
#undef BEGIN_MENU
#undef END_MENU
#undef ADD_CMD_SECRET
#undef ADD_MENU_SECRET
#undef ADD_LINK

/**************************************************************/
/* Define menu-struct for linkage                             */
/**************************************************************/
#define BEGIN_MENU(name) struct CLI_MENU_STRUCT(name) { 
#define END_MENU UINT32_T CLI_MENU_STRUCT_FIELD(__end__); };
#define ADD_CMD_SECRET(name, handler_func, desc, level) UINT32_T CLI_MENU_STRUCT_FIELD(name); 
#define ADD_MENU_SECRET(name, menu_name, desc, level) UINT32_T CLI_MENU_STRUCT_FIELD(name); 
#define ADD_LINK(name, menu, item, level) UINT32_T __cli_dummy_menu_struct_field_##name; 
#include "../app/integrate/cli_global/menu.h"
#undef BEGIN_MENU
#undef END_MENU
#undef ADD_CMD_SECRET
#undef ADD_MENU_SECRET
#undef ADD_LINK

/**************************************************************/
/* Define menu item arrays                                    */
/**************************************************************/
#define BEGIN_MENU(name) static CLI_ITEM_T CLI_MENU_ITEMS(name)[] = {
#define END_MENU {NULL, NULL, CMD, 0, NULL, CLI_MENU_ID_MAX}};
#ifdef CLI_ENABLE_PASSWORD
    #define ADD_CMD_SECRET(name, handler_func, desc, level) {#name, desc, CMD, CLI_LEVEL_ENUM_##level, handler_func, CLI_MENU_ID_MAX},
    #define ADD_MENU_SECRET(name, menu_name, desc, level) {#name, "menu: " desc, MENU, CLI_LEVEL_ENUM_##level, NULL, CLI_MENU_ENUM(menu_name)},
    #define ADD_LINK(name, menu, item, level) {#name, NULL, LINK, CLI_LEVEL_ENUM_##level, NULL, CLI_LINK(menu, item)}, 
#else
#define ADD_CMD_SECRET(name, handler_func, desc, level) {#name, desc, CMD, 0, handler_func, CLI_MENU_ID_MAX},
#define ADD_MENU_SECRET(name, menu_name, desc, level) {#name, "menu: " desc, MENU, 0, NULL, CLI_MENU_ENUM(menu_name)},
#define ADD_LINK(name, menu, item, level) {#name, NULL, LINK, 0, NULL, CLI_LINK(menu, item)}, 
#endif
#include "../app/integrate/cli_global/menu.h"
#include "cli_builtin_menu.h"
#undef BEGIN_MENU
#undef END_MENU
#undef ADD_CMD_SECRET
#undef ADD_MENU_SECRET
#undef ADD_LINK

/**************************************************************/
/* Define menu objects, connected with menu item arrays       */
/**************************************************************/
static CLI_MENU_T cli_menus[] = {
#define BEGIN_MENU(name) { #name, CLI_MENU_ITEMS(name) },
#define END_MENU
#define ADD_CMD_SECRET(name, handler_func, desc, level)
#define ADD_MENU_SECRET(name, menu_name, desc, level)
#define ADD_LINK(name, menu, item, level) 
#include "../app/integrate/cli_global/menu.h"
#include "cli_builtin_menu.h"
#undef BEGIN_MENU
#undef END_MENU
#undef ADD_CMD_SECRET
#undef ADD_MENU_SECRET
#undef ADD_LINK
		};

/**************************************************************/
/* Global Variable Definition                                 */
/**************************************************************/
static CLI_ITEM_T *menu_path[MAX_PATH_DEPTH];
static CLI_ITEM_T root_item = { "", "root", MENU, 0, NULL, CLI_MENU_ENUM(root) };
static CLI_ITEM_T *current_item = &root_item;
static BOOL_T executing_batch = FALSE;
static CLI_BATCH_T *batch_stack[MAX_BATCH_LEVEL];

static int depth = 0;
static int batch_level = -1;
static int current_secret_level = 0;
static int max_secret_level = 0;
#ifdef CLI_ENABLE_PASSWORD
static const int password_count = sizeof(passwords)/sizeof(char*) - 1;
#endif
static BOOL_T password_enabled = TRUE;
static BOOL_T rand_seeded = FALSE;

// for passing result
static result_list_t *result_set = NULL;
static result_list_t *result_set_last = NULL;
static char *result_data = NULL;
static int result_status = 0;
static char *cmd_id = NULL;

/**************************************************************/
/* Static Function Declaration                                */
/**************************************************************/
static BOOL_T parseInt(const char *intStr, UINT32_T *result, int radix);
static void addBatchToBatch(CLI_BATCH_T *parent, CLI_BATCH_T *child);
static void addCmdToBatch(CLI_BATCH_T *batch, CLI_ITEM_T *cmd, int argc,
		char *argv[]);
static BOOL_T executeBatch(CLI_BATCH_T *batch);
#if defined(THREAD_SUPPORT)
static BOOL_T executeBatchParallel(CLI_BATCH_T* batch);
#endif
static BOOL_T executeBatchSeq(CLI_BATCH_T *batch);
static BOOL_T executeBatchRandomOne(CLI_BATCH_T *batch);
static void shuffleBatch(CLI_BATCH_T *batch);
static void releaseBatch(CLI_BATCH_T *batch);

/**************************************************************/
/* Function Implementation                                    */
/**************************************************************/
void CLI_InitMenu(void) {
	int i;

	/** Clean to zero **/
	memset(menu_path, 0x00, MAX_PATH_DEPTH * sizeof(CLI_ITEM_T*));
	memset(batch_stack, 0x00, MAX_BATCH_LEVEL * sizeof(CLI_BATCH_T*));

	/** init history **/
	history_init();

	for (i = 0; i < sizeof(cli_menus) / sizeof(cli_menus[0]); i++) {
		CLI_ITEM_T *p_item = cli_menus[i].items;
		while (p_item->name != NULL) {
#ifdef CLI_ENABLE_PASSWORD
            int level = p_item->secret_level;
            if(level > max_secret_level) max_secret_level = level;
#endif
			if (p_item->type == LINK) {
				CLI_ITEM_T *target =
						&cli_menus[p_item->menu & 0xFFFF].items[((p_item->menu
								>> 16) & 0xFFFF)];
				p_item->type = target->type;
				p_item->desc = target->desc;
				if (target->type == CMD)
					p_item->handler = target->handler;
				else
					// must be MENU type
					p_item->menu = target->menu;
			}
			p_item++;
		}
	}
}

#undef ADD_CMD
#undef ADD_MENU

static void dispatchCmd(char *cmd_name, int argc, char *args[]);
static CLI_ITEM_T* findItem(CLI_MENU_T *menu, const char *name);
static CLI_ITEM_T* findPath(const char *name, BOOL_T cd);
static void output_cli_result(BOOL_T executed);

void CLI_Execute(const char *cmd) {
	const char delims[] = " \x0d\x0a";
	char *args[40] = { 0 };
	char *p = NULL, *cmd_name = NULL;
	int argc = 0;

	char *cmd_copied = NULL;
	int len = 0;

	len = strlen(cmd);
	len++;

	cmd_copied = (char*) malloc((len) * sizeof(char));
	if (cmd_copied == NULL)
		return;

	cmd_id = NULL;
	strcpy(cmd_copied, cmd);

	p = strtok(cmd_copied, delims);
	if (p == NULL || *p == '#') {
		return;
	}

	if (p[0] == '@' && p[1] != '\0') {
		cmd_id = p + 1;
		p = strtok(NULL, delims);
		if (p == NULL || *p == '#') {
			output_cli_result(TRUE);
			return;
		}
	}
	cmd_name = p;

	p = strtok(NULL, delims);
	while (p != NULL && *p != '#') {
		args[argc] = p;
		argc++;
		if (argc == sizeof(args) / sizeof(char*))
			break;
		p = strtok(NULL, delims);
	}

	dispatchCmd(cmd_name, argc, args);

	free(cmd_copied);
}

void CLI_GetCurrentPath(char *pwd, int pwd_size) {
	int i;
	int remain_size = pwd_size;
	char *p = pwd;

	for (i = 0; i < depth; i++) {
		int n_write = snprintf(p, remain_size, "%s/", menu_path[i]->name);
		if (n_write >= remain_size)
			return;

		p += n_write;
		remain_size -= n_write;
	}
	snprintf(p, remain_size, "%s/", current_item->name);
}

void CLI_AddResultData(const char *fmt, ...) {
	va_list args;
	int str_len;
	char *buf;
	result_list_t *next_result;

	if (executing_batch)
		return; // batched command can't return result
	if (cmd_id == NULL)
		return;

	// obtain the buffer size first
	va_start(args, fmt);
	str_len = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	// allocate that large a buffer
	buf = malloc((str_len + 1) * sizeof(char));

	// do sprintf it
	va_start(args, fmt);
	vsnprintf(buf, str_len + 1, fmt, args);
	va_end(args);

	next_result = (result_list_t*) malloc(sizeof(result_list_t));
	next_result->next = NULL;
	next_result->value = buf;

	if (result_set == NULL) {
		result_set = next_result;
		result_set_last = next_result;
	} else {
		result_set_last->next = next_result;
		result_set_last = next_result;
	}
}

void CLI_SetResultData(const char *fmt, ...) {
	va_list args;
	int str_len;

	if (executing_batch)
		return;
	if (cmd_id == NULL)
		return;

	// obtain the buffer size first
	va_start(args, fmt);
	str_len = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	// allocate that large a buffer
	if (result_data != NULL)
		free(result_data);
	result_data = malloc((str_len + 1) * sizeof(char));

	// do sprintf it
	va_start(args, fmt);
	vsnprintf(result_data, str_len + 1, fmt, args);
	va_end(args);
}

void CLI_SetStatusCode(int status) {
	if (executing_batch)
		return;
	if (cmd_id == NULL)
		return;

	result_status = status;
}

static void output_cli_result(BOOL_T executed) {
	if (executing_batch)
		return;
	if (cmd_id == NULL)
		return;

	cprintf("\n$(%s,%d,%d,", cmd_id, executed, executed ? result_status : 0);

	if (result_set != NULL) {
		result_list_t *r = result_set;

		cprintf("(");
		while (r != NULL) {
			result_list_t *to_free;
			cprintf("%s,", r->value);
			free(r->value);

			to_free = r;
			r = r->next;
			free(to_free);
		}
		cprintf(")");
	} else if (result_data != NULL) {
		cprintf("%s", result_data);
	}

	cprintf(")\n");

	if (result_data != NULL) {
		free(result_data);
	}

	result_data = NULL;
	result_set = NULL;
	result_set_last = NULL;
	result_status = 0;
}

static BOOL_T executeHandler(CLI_ITEM_T *p_item, int argc, char *args[]) {
	ASSERT(p_item != NULL);

	if (!p_item->handler(argc, args)) {
		cprintf("Usage: \n%s\n", p_item->desc);
		CLI_SetResultData("Wrong usage");
		output_cli_result(FALSE);
		return FALSE;
	}

	output_cli_result(TRUE);
	return TRUE;
}

static void dispatchCmd(char *cmd_name, int argc, char *args[]) {
	CLI_ITEM_T *p_item;

	// search for menu-specific cmd first
	p_item = findPath(cmd_name, FALSE);
	if (p_item != NULL && p_item->type == CMD) {
		goto user_cmd_found;
	}

	// search for user-defined global cmd next
	p_item = findItem(&CLI_MENU(global), cmd_name);
	if (p_item != NULL && p_item->type == CMD) {
		goto user_cmd_found;
	}

	// search for builtin cmd finally
	p_item = findItem(&CLI_MENU(builtin), cmd_name);
	if (p_item != NULL && p_item->type == CMD) {
		// builtin commands are executed regardless of the batch mode 
		executeHandler(p_item, argc, args);
		return;
	}

	cprintf("Unkonwn command\n");
	CLI_SetResultData("Unkonwn command");
	CLI_SetStatusCode(0);
	output_cli_result(FALSE);
	return;

	user_cmd_found: if (batch_level == -1) {
		executeHandler(p_item, argc, args);
	} else {
		addCmdToBatch(batch_stack[batch_level], p_item, argc, args);
		CLI_SetResultData("Command added to batch");
		output_cli_result(TRUE);
	}
}

static CLI_ITEM_T* findPath(const char *name, BOOL_T cd) {
	CLI_ITEM_T *p_item;
	const char delims[] = "/";
	CLI_ITEM_T *path[MAX_PATH_DEPTH];
	int path_depth;
	char *p;
	char buf[128];

#if HX_HW_TARGET == HX_HW_SIM
	memset(buf, 0, sizeof(buf));
#endif    

	if (strlen(name) >= sizeof(buf))
		return NULL;

	strcpy(buf, name);

	if (buf[0] == '/') {
		p_item = &root_item;
		path_depth = 0;
	} else {
		p_item = current_item;
		path_depth = depth;
		memcpy(path, menu_path, depth * sizeof(CLI_ITEM_T*));
	}

	p = strtok(buf, delims);

	while (p != NULL) {
		if (!strcmp("..", p)) {
			if (path_depth != 0) {
				path_depth--;
				p_item = path[path_depth];
			}
		} else {
			CLI_ITEM_T *child = findItem(&cli_menus[p_item->menu], p);

			if (child == NULL) {
				return NULL;
			}

			if (path_depth == MAX_PATH_DEPTH) {
				cprintf("Path too deep\n");
				return NULL;
			}

			path[path_depth] = p_item;
			path_depth++;
			p_item = child;
		}

		p = strtok(NULL, delims);

		if (p != NULL && p_item->type != MENU) {
			// in the middle of the path but not a menu
			return NULL;
		}
	}

	if (cd) {
		if (p_item->type != MENU) {
			return NULL;
		}

		current_item = p_item;
		depth = path_depth;
		memcpy(menu_path, path, depth * sizeof(CLI_ITEM_T*));
	}

	return p_item;
}

static CLI_ITEM_T* findItem(CLI_MENU_T *menu, const char *name) {
	CLI_ITEM_T *p_item;

	for (p_item = &menu->items[0]; p_item->name != NULL; p_item++) {
		if (!strcmp(p_item->name, name))
			break;
	}

	if (p_item->name == NULL) {
		return NULL;
	} else if (password_enabled
			&& p_item->secret_level > current_secret_level) {
		return NULL;
	} else {
		return p_item;
	}
}

static BOOL_T cli_builtin_help(int argc, char *argv[]) {
	CLI_ITEM_T *p_item;

	if (argc > 1) {
		return FALSE;
	}

	if (argc == 0) {
		for (p_item = &CLI_MENU(global).items[0]; p_item->name != NULL;
				p_item++) {
			cprintf("%s\t\t%s\n", p_item->name, p_item->desc);
		}
		for (p_item = &CLI_MENU(builtin).items[0]; p_item->name != NULL;
				p_item++) {
			cprintf("%s\t\t%s\n", p_item->name, p_item->desc);
		}
	} else {
		char *topic = argv[0];

		p_item = findPath(topic, FALSE);

		if (p_item == NULL)
			p_item = findItem(&CLI_MENU(global), topic);

		if (p_item == NULL)
			p_item = findItem(&CLI_MENU(builtin), topic);

		if (p_item == NULL)
			cprintf("Command or menu not found: %s\n", topic);
		else
			cprintf("%s\t%s\n", p_item->name, p_item->desc);
	}

	return TRUE;

}

static BOOL_T cli_builtin_ls(int argc, char *argv[]) {
	CLI_ITEM_T *p_item;

	if (argc > 0) {
		p_item = findPath(argv[0], FALSE);
	} else {
		p_item = current_item;
	}

	if (p_item == NULL) {
		cprintf("Item not found\n");
		return TRUE;
	}

	if (p_item->type == CMD) {
		cprintf("%s\n", p_item->name);
	} else {
		CLI_ITEM_T *p;
		for (p = &cli_menus[p_item->menu].items[0]; p->name != NULL; p++) {
			if (p->type != MENU)
				continue;
			if (!password_enabled
					|| (password_enabled
							&& p->secret_level <= current_secret_level))
				cprintf("%s/\n", p->name);
		}

		for (p = &cli_menus[p_item->menu].items[0]; p->name != NULL; p++) {
			if (p->type != CMD)
				continue;
			if (!password_enabled
					|| (password_enabled
							&& p->secret_level <= current_secret_level))
				cprintf("%s\n", p->name);
		}
	}

	return TRUE;
}

static BOOL_T cli_builtin_pwd(int argc, char *argv[]) {
	char buf[128];

	CLI_GetCurrentPath(buf, sizeof(buf));
	cprintf("%s\n", buf);
	return TRUE;
}

static BOOL_T cli_builtin_cd(int argc, char *argv[]) {
	char *dir;

	if (argc == 0) // default cd to top
			{
		current_item = &root_item;
		depth = 0;
		return TRUE;
	} else if (argc > 1) {
		return FALSE;
	}

	dir = argv[0];

	if (findPath(dir, TRUE) == NULL) {
		cprintf("Menu not found: %s\n", dir);
	}

	return TRUE;
}

static BOOL_T cli_builtin_begin(int argc, char *argv[]) {
	int i;
	CLI_BATCH_T *batch;
	uint32_t tick, loop_cnt;

	// create and initialize a new batch object
	batch = (CLI_BATCH_T*) malloc(sizeof(CLI_BATCH_T));
	batch->exe_manner = SEQUENTIAL;
	batch->items_head = NULL;
	batch->items_tail = NULL;
	batch->loop_count = 1;
	batch->item_count = 0;

	// determine the batch executing manner
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "random") == 0) {
			batch->exe_manner |= RANDOM;
			if (!rand_seeded) {
    			SystemGetTick(&tick, &loop_cnt);
            	srand(tick);
				rand_seeded = TRUE;
			}
		} else if (strcmp(argv[i], "parallel") == 0) {
#if defined(THREAD_SUPPORT)
            batch->exe_manner |= PARALLEL;
#else
			cprintf("threading not supported\n");
			free(batch);
			return TRUE;
#endif
		} else if (strcmp(argv[i], "loop") == 0) {
			i++;
			if (i == argc)
				goto error;

			if (!parseInt(argv[i], (UINT32_T*) &batch->loop_count, 10))
				goto error;
		} else if (strcmp(argv[i], "random-one") == 0) {
			batch->exe_manner |= RANDOM_ONE;
			if (!rand_seeded) {
    			SystemGetTick(&tick, &loop_cnt);
            	srand(tick);
				rand_seeded = TRUE;
			}
		} else {
			goto error;
		}
	}

	// random-one overrides random and parallel
	if (batch->exe_manner & RANDOM_ONE)
		batch->exe_manner &= ~(RANDOM | PARALLEL);

	// set this batch as the current batch
	batch_level++;
	if (batch_level == MAX_BATCH_LEVEL) {
		free(batch);
		cprintf("batch level exceeds the limit MAX_BATCH_LEVEL (%d)\n",
				MAX_BATCH_LEVEL);
		return TRUE;
	}
	batch_stack[batch_level] = batch;

	cprintf("batch level %d\n", batch_level);

	return TRUE;

	error: free(batch);
	return FALSE;
}

static void addItemToBatch(CLI_BATCH_T *batch, CLI_BATCH_ITEM_T *item) {
	item->next = NULL;

	if (batch->items_tail == NULL) {
		// the batch has no item and needs initialization
		batch->items_head = item;
		batch->items_tail = item;
	} else {
		batch->items_tail->next = item;
		batch->items_tail = item;
	}
	batch->item_count++;
}

static void addBatchToBatch(CLI_BATCH_T *parent, CLI_BATCH_T *child) {
	CLI_BATCH_ITEM_T *item = (CLI_BATCH_ITEM_T*) malloc(
			sizeof(CLI_BATCH_ITEM_T));
	item->type = BATCH;
	item->item.batch = child;

	addItemToBatch(parent, item);
}

static void addCmdToBatch(CLI_BATCH_T *batch, CLI_ITEM_T *cli_item, int argc,
		char *argv[]) {
	int i;
	CLI_BATCH_ITEM_T *item;
	CLI_BATCHED_CMD_T *cmd;

	cmd = (CLI_BATCHED_CMD_T*) malloc(sizeof(CLI_BATCHED_CMD_T));
	cmd->cmd_item = cli_item;
	cmd->argc = argc;
	cmd->argv = (char**) malloc(argc * sizeof(char*));
	for (i = 0; i < argc; i++) {
		cmd->argv[i] = (char*) malloc((strlen(argv[i]) + 1) * sizeof(char));
		strcpy(cmd->argv[i], argv[i]);
	}

	item = (CLI_BATCH_ITEM_T*) malloc(sizeof(CLI_BATCH_ITEM_T));
	item->type = SINGLE;
	item->item.cmd = cmd;

	addItemToBatch(batch, item);
	cprintf("add command to batch(level %d)\n", batch_level);
}

static BOOL_T cli_builtin_end(int argc, char *argv[]) {
	if (batch_level == -1)
		return TRUE;

	batch_level--;

	if (batch_level != -1) {
		// sub batch is added to its parent when it is closed by `end'
		addBatchToBatch(batch_stack[batch_level], batch_stack[batch_level + 1]);
		cprintf("batch level %d\n", batch_level);
		return TRUE;
	}

	// batch level 0 closed; execute the whole thing
	cprintf("begin to execute the batch\n");
	executing_batch = TRUE;
	executeBatch(batch_stack[0]);
	releaseBatch(batch_stack[0]);
	executing_batch = FALSE;
	cprintf("batch execution done\n");
	return TRUE;
}

static BOOL_T executeBatch(CLI_BATCH_T *batch) {
	int i;

	if (batch->item_count == 0)
		return TRUE;

	for (i = 0; i < batch->loop_count; i++) {
		cprintf("Loop #%d\n", i);
		if (batch->exe_manner & RANDOM_ONE) {
			if (!executeBatchRandomOne(batch))
				return FALSE;
			continue;
		}

		if (batch->exe_manner & RANDOM) {
			shuffleBatch(batch);
		}

		if (batch->exe_manner & PARALLEL) {
#if defined(THREAD_SUPPORT)
            if(!executeBatchParallel(batch)) return FALSE;
#else
			ASSERT("parallel should have been prohibited");
#endif
		} else {
			if (!executeBatchSeq(batch))
				return FALSE;
		}
	}
	return TRUE;
}

static BOOL_T executeBatchItem(CLI_BATCH_ITEM_T *item) {
	if (item->type == BATCH) {
		return executeBatch(item->item.batch);
	} else {
		int i;
		CLI_BATCHED_CMD_T *cmd = item->item.cmd;
		cprintf("+ %s ", cmd->cmd_item->name);
		for (i = 0; i < cmd->argc; i++) {
			cprintf("%s ", cmd->argv[i]);
		}
		cprintf("\n");

		return executeHandler(cmd->cmd_item, cmd->argc, cmd->argv);
	}
}

#if defined(THREAD_SUPPORT)

static void* batchThreadProc(void *arg)
{
    CLI_BATCH_ITEM_T* item = (CLI_BATCH_ITEM_T*) arg;
    
    executeBatchItem(item);
    return NULL;
}

static BOOL_T executeBatchParallel(CLI_BATCH_T* batch)
{
    int i;
    int thread_count = 0;
    BOOL_T has_error = FALSE;
    OSW_THREAD_T* ths = (OSW_THREAD_T*) malloc(batch->item_count*sizeof(OSW_THREAD_T));
    CLI_BATCH_ITEM_T* item;
    
    for(item = batch->items_head; item != NULL; item = item->next)
    {
        STATUS_T ret = 
            OSW_CreateJoinableThread(
                &ths[thread_count], batchThreadProc, BATCH_THREAD_STACK_SIZE, OSW_PRIORITY_NORMAL, item);
        
        if(ret != STATUS_RETURN_SUCCESS)
        {
            cprintf("Failed to create thread: %x\n", ret);
            has_error = TRUE;
            break;
        }
        else
        {
            thread_count++;
        }
    }
    
    for(i=0; i<thread_count; i++)
    {
        // wait for all threads to finish
        if(OSW_ThreadJoin(&ths[i]) != STATUS_RETURN_SUCCESS)
        {
            cprintf("Failed to join a thread\n");
            has_error = TRUE;
            break;
        }
    } 

    free(ths);
    return !has_error;
}

#endif // THREAD_SUPPORT

static BOOL_T executeBatchSeq(CLI_BATCH_T *batch) {
	CLI_BATCH_ITEM_T *item;

	for (item = batch->items_head; item != NULL; item = item->next) {
		if (!executeBatchItem(item))
			return FALSE;
	}
	return TRUE;
}

static BOOL_T executeBatchRandomOne(CLI_BATCH_T *batch) {
	int i;
	int the_one = rand() % batch->item_count;
	CLI_BATCH_ITEM_T *item = batch->items_head;

	for (i = 0; i < the_one; i++) {
		item = item->next;
	}
	return executeBatchItem(item);
}

static void shuffleBatch(CLI_BATCH_T *batch) {
	int i;
	CLI_BATCH_ITEM_T **p_items = (CLI_BATCH_ITEM_T**) malloc(
			batch->item_count * sizeof(CLI_BATCH_ITEM_T*));
	CLI_BATCH_ITEM_T *item = batch->items_head;

	// copy items to an array to make shuffling easier
	for (i = 0; i < batch->item_count; i++, item = item->next) {
		p_items[i] = item;
	}

	// perform the shuffling
	for (i = batch->item_count; i > 1; i--) {
		CLI_BATCH_ITEM_T *tmp;
		int rand_ind = rand() % i;

		//swap
		tmp = p_items[rand_ind];
		p_items[rand_ind] = p_items[i - 1];
		p_items[i - 1] = tmp;
	}

	// reconstruct the linked-list according to the shuffled array
	batch->items_head = p_items[0];
	for (i = 1; i < batch->item_count; i++) {
		p_items[i - 1]->next = p_items[i];
	}
	batch->items_tail = p_items[batch->item_count - 1];
	batch->items_tail->next = NULL;

	free(p_items);
}

static void releaseBatch(CLI_BATCH_T *batch) {
	CLI_BATCH_ITEM_T *item;
	CLI_BATCH_ITEM_T *next_item;

	for (item = batch->items_head; item != NULL; item = next_item) {
		next_item = item->next;

		if (item->type == SINGLE) {
			int i;
			for (i = 0; i < item->item.cmd->argc; i++) {
				free(item->item.cmd->argv[i]);
			}
			free(item->item.cmd->argv);
			free(item->item.cmd);
		} else {
			releaseBatch(item->item.batch);
		}
		free(item);
	}

	free(batch);
}

static BOOL_T cli_builtin_cancel(int argc, char *argv[]) {
	if (batch_level == -1)
		return TRUE;

	releaseBatch(batch_stack[batch_level]);
	batch_level--;

	if (batch_level == -1) {
		cprintf("batch mode ends\n");
	} else {
		cprintf("cancelled; back to batch level %d\n", batch_level);
	}

	return TRUE;
}

static BOOL_T cli_builtin_load(int argc, char *argv[]) {
	FILE *script;
	char buf[256];

	if (argc != 1)
		return FALSE;

	script = fopen(argv[0], "r");
	if (script == NULL) {
		cprintf("Failed to open file `%s'\n", argv[0]);
		return TRUE;
	}

	while (fgets(buf, sizeof(buf), script) != NULL) {
		CLI_Execute(buf);
	}
	return TRUE;
}

static BOOL_T cli_builtin_level(int argc, char *argv[]) {
	UINT32_T level;

	if (argc == 0) {
		cprintf("Max level: %d; Current: %d\n", max_secret_level,
				current_secret_level);
		return TRUE;
	} else if (argc > 1) {
		return FALSE;
	}

	if (!password_enabled) {
		cprintf("Password not enabled\n");
		return TRUE;
	}

	// change level
	if (!parseInt(argv[0], &level, 10))
		return FALSE;
	if (level > max_secret_level) {
		cprintf("Max secret level is %d\n", max_secret_level);
		return TRUE;
	}
#ifdef CLI_ENABLE_PASSWORD
    if(level > current_secret_level && password_count >= level) // password needed and defined
    {
        int c;
        char buf[64];
        int charCnt = 0;
        cprintf("Password: ");
        while(charCnt < sizeof(buf)-1 && (c=readchar()) != '\n' && c != '\r')
        {
            buf[charCnt++] = (char) c;
        }
        buf[charCnt] = '\0';
        if(strcmp(buf, passwords[level]) != 0)
        {
            cprintf("Incorrect password\n");
            return TRUE;
        }
    }
    else // cd back to the toppest dir that is reachable for the new level
#endif
	{
		int i;
		for (i = 0; i < depth; i++) {
			if (menu_path[i]->secret_level > level)
				break;
		}

		if (i < depth || current_item->secret_level > level) {
			// i>=1 must hold, since menu_path[0] is root, of which the secret level is 0.
			depth = i - 1;
			current_item = menu_path[depth];
		}
	}
	current_secret_level = level;
	return TRUE;
}

static BOOL_T parseInt(const char *intStr, UINT32_T *result, int radix) {
	char *endp;
	*result = strtoul(intStr, &endp, radix);

	if (endp != intStr + strlen(intStr)) {
		return FALSE;
	}

	return TRUE;
}

void CLI_EnablePassword(BOOL_T yes) {
#ifdef CLI_ENABLE_PASSWORD
    password_enabled = yes;
#endif
}

