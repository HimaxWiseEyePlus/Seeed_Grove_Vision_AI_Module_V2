/**************************************************************************************************
 (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
 ------------------------------------------------------------------------
 File        : readline.c
 Project     : WEI
 DATE        : 2013/03/01
 AUTHOR      : 300212
 BRIFE       : This file implements the parser for reading whole string line
 HISTORY     : Initial version - 2013/03/01 created by will
 **************************************************************************************************/
#include <string.h>
#include "cli_type.h"
#include "cli_debug.h"
#include "cli_console.h"
#include "history.h"

#define CTRL(chr) (chr-'A'+'\x01')

#define CUR_RIGHT(n) "\033[" #n "C"  
#define CUR_LEFT(n)  "\033[" #n "D"
#define CR "\x0d"
#define ERASE_EOL "\033[K"
#define ERASE_LINE "\033[2K"
#define BACKSPACE "\x08"

// the following control seq's have been given up since hyper-terminal doesn't support them.
#define CUR_HOR(col) "\033[" #col "G"  
#define INSERT_MODE "\033[4h"
#define OVERWRITE_MODE "\033[4l"

int readline(char *line, UINT32_T size, const char *prompt) {
	int charCnt = 0;
	int cursorCol = 0;
	int promptLen;
	BOOL_T dirty = FALSE;
	enum ESC_MODE {
		ESC_NONE, ESC_PHASE1, ESC_PHASE2
	} escMode = ESC_NONE;
	BOOL_T orig_signal_enabled;

	DbgPrintfSuppress(TRUE);
	Console_EnableSignal(FALSE, &orig_signal_enabled);

	promptLen = strlen(prompt);
	cprintf("\n%s", prompt);

	while (TRUE) {
		int c;

		c = readchar();

		if (escMode == ESC_PHASE1) {
			if (c == '[') {
				escMode = ESC_PHASE2;
			} else {
				escMode = ESC_NONE;
			}
		} else if (escMode == ESC_PHASE2) {
			const char *history;
			switch (c) {
			case 'A':  // up
				if (dirty) {
					line[charCnt] = '\0';
					history_edit(line);
					dirty = FALSE;
				}

				history = history_previous();
				if (history != NULL) {
					strncpy(line, history, size - 1);
					line[size - 1] = '\0';
					charCnt = strlen(line);
					cursorCol = charCnt;
					cprintf(ERASE_LINE CR "%s%s" CR CUR_RIGHT(%d), prompt, line,
							cursorCol + promptLen);
				}
				break;
			case 'B':  // down
				if (dirty) {
					line[charCnt] = '\0';
					history_edit(line);
					dirty = FALSE;
				}

				history = history_next();
				if (history != NULL) {
					strncpy(line, history, size - 1);
					line[size - 1] = '\0';
					charCnt = strlen(line);
					cursorCol = charCnt;
					cprintf(ERASE_LINE CR "%s%s" CR CUR_RIGHT(%d), prompt, line,
							cursorCol + promptLen);
				}
				break;
			case 'C':  // right
				if (cursorCol < charCnt) {
					cursorCol++;
					cprintf(CUR_RIGHT(1));
				}
				break;
			case 'D':  // left
				if (cursorCol > 0) {
					cursorCol--;
					cprintf(CUR_LEFT(1));
				}
				break;
			}

			escMode = ESC_NONE;
		} else // non-esc
		{
			if (c == '\x0d') // newline
					{
				goto done;
			}
#ifdef SEMIHOST
            if(c == '\x0a') // newline
            {
                goto done;
            }
#endif
			else if (c == '\x08') // backspace
					{
				if (charCnt > 0) {
					charCnt--;
					cursorCol--;
					dirty = TRUE;

					if (cursorCol == charCnt) {
						cprintf(BACKSPACE " " BACKSPACE);
					} else {
						memmove(&line[cursorCol], &line[cursorCol + 1],
								charCnt - cursorCol);
						line[charCnt] = '\0';
						cprintf(ERASE_EOL BACKSPACE "%s" CR CUR_RIGHT(%d),
								line + cursorCol, cursorCol + promptLen);
					}
				}
			} else if (c == '\033') // ESC
					{
				escMode = ESC_PHASE1;
			} else if (c == CTRL('A')) // goto start of line
			{
				cursorCol = 0;
				cprintf(CR CUR_RIGHT(%d), promptLen);
			} else if (c == CTRL('E')) // goto end of line
			{
				cursorCol = charCnt;
				cprintf(CR CUR_RIGHT(%d), cursorCol + promptLen);
			} else if (c == CTRL('C')) // give up current line
			{
				cprintf("\n%s", prompt);
				cursorCol = charCnt = 0;
				dirty = FALSE;
				history_forget_editing();
			} else if (c == CTRL('K')) // erase to end-of-line
			{
				if (cursorCol != charCnt) {
					dirty = TRUE;
					charCnt = cursorCol;
					cprintf(ERASE_EOL);
				}
			} else if (c == CTRL('U')) // erase the whole line
			{
				cursorCol = charCnt = 0;
				cprintf(ERASE_LINE CR "%s", prompt);
			} else if (c == CTRL('W')) // erase to start-of-line
			{
				if (cursorCol != 0) {
					dirty = TRUE;
					charCnt -= cursorCol;
					memmove(line, &line[cursorCol], charCnt);
					line[charCnt] = '\0';
					cursorCol = 0;
					cprintf(ERASE_LINE CR "%s%s" CR CUR_RIGHT(%d), prompt, line,
							promptLen);
				}
			} else if (c == CTRL('L')) // refresh
			{
				line[charCnt] = '\0';
				cprintf(ERASE_LINE CR "%s%s" CR CUR_RIGHT(%d), prompt, line,
						cursorCol + promptLen);
			} else if (c >= 0x20) // all printable character
					{
				if (charCnt < size - 1) {
					dirty = TRUE;
					if (cursorCol == charCnt) {
						line[charCnt++] = c;
						cprintf("%c", c);
						cursorCol++;
					} else {
						memmove(&line[cursorCol + 1], &line[cursorCol],
								charCnt - cursorCol);
						line[cursorCol] = c;
						charCnt++;
						line[charCnt] = '\0';
						cprintf("%s" CR CUR_RIGHT(%d), line + cursorCol,
								cursorCol + 1 + promptLen);
						cursorCol++;
					}
				} else {
					// buffer full; beeps
					cprintf("%c", 7);
				}
			}
		}
	}

	done:
	cprintf("\n");
	line[charCnt] = '\0';
	if (charCnt > 0)
		history_add(line);
	history_forget_editing();
	Console_EnableSignal(orig_signal_enabled, NULL);
	DbgPrintfSuppress(FALSE);
	return charCnt;
}

