#ifndef __CLI_CONSOLE_H__
#define __CLI_CONSOLE_H__

#include "cli_type.h"

int readline(char *line, UINT32_T size, const char *prompt);

#define DUMMY_EnableSignal(yes, p_orig) do { BOOL_T* tmp = (p_orig); if(tmp != NULL) *tmp = FALSE; } while(0)

#if defined(UART_CONSOLE)
// modified by HMX@Jacky - Update porting API for WEI platform (from source console_io and xprintf)
#ifdef PROJECT_EPII
	#include "xprintf.h"
	#include "console_io.h"

	#define keyin console_getchar
	#define readchar console_getchar
	#define cprintf(fmt, ...)  xprintf(fmt, ##__VA_ARGS__)
	#define Console_InitTerminal()
	#define Console_RestoreTerminal()
	#define Console_EnableSignal DUMMY_EnableSignal
#endif
#elif defined(TUBE_CONSOLE) || defined(SEMI_HOST_CONSOLE) || defined(WINDOWS_CONSOLE) || defined(NO_CONSOLE)
    #define keyin() (-1)
    #define readchar() (-1)
    #include "com_debug.h"
    #define cprintf(fmt, ...)  DbgPrintf(fmt, ##__VA_ARGS__)
    #define Console_InitTerminal()
    #define Console_RestoreTerminal()
    #define Console_EnableSignal DUMMY_EnableSignal
#else
#include <stdio.h>
int keyin(void);
int readchar(void);
#include "com_debug.h"
#define cprintf(fmt, ...) printf(fmt, ##__VA_ARGS__)
void Console_InitTerminal(void);
void Console_RestoreTerminal(void);
void Console_EnableSignal(BOOL_T yes, BOOL_T *p_orig);
#endif

#endif /*__CLI_CONSOLE_H__*/

