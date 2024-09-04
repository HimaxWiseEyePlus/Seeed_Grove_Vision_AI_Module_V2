/*
 * print_x.h
 *
 * CGP - 27/5/22
 *
 * Experiment in replacing my_printf with PR_DEBUG etc.
 *
 * Based on this:
 * D:\Development\MAX78000_SDK-11Apr22\Examples\MAX78000\CNN\faceid_evkit\include\MAXCAM_Debug.h
 *
 * Looks like we can define a message priority level and print this hierachy:
 * (DEBUG,INFO,WARN,ERROR,FATAL).
 *
 */

#ifndef APP_SCENARIO_APP_I2C_SLAVE_TEST_PRINT_X_H_
#define APP_SCENARIO_APP_I2C_SLAVE_TEST_PRINT_X_H_

#include <stdio.h>
#include <string.h>
#include "xprintf.h"

/* Stringification macros */
#define STRING(x) STRING_(x)
#define STRING_(x) #x

// CGP I added this to enable or disable the PR_DEBUG messages.
// Enable one of these
#define PRINT_PRIORITY	0	// print DEBUG, INFO, WARN, ERR
//#define PRINT_PRIORITY	1	// print INFO, WARN, ERR
//#define PRINT_PRIORITY	2	// print WARN, ERR
//#define PRINT_PRIORITY	3	// print ERR
//#define PRINT_PRIORITY	  4	// print NONE MESSAGES

// See here for colours: https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
// https://wiki.bash-hackers.org/scripting/terminalcodes
#define ESC_RED "\033[1;31m"
#define ESC_GREEN "\033[1;32m"
#define ESC_YELLOW "\033[1;33m"
#define ESC_BLUE "\033[1;34m"
#define ESC_MAGENTA "\033[1;35m"
#define ESC_CYAN "\033[1;36m"
#define ESC_WHITE "\033[0m"

#define P_RED		xprintf("%s", ESC_RED);
#define P_GREEN		xprintf("%s", ESC_GREEN);
#define P_YELLOW	xprintf("%s", ESC_YELLOW);
#define P_BLUE		xprintf("%s", ESC_BLUE);
#define P_MAGENTA	xprintf("%s", ESC_MAGENTA);
#define P_CYAN		xprintf("%s", ESC_CYAN);
#define P_WHITE		xprintf("%s", ESC_WHITE);

//#ifdef PRINT_PRIORITY
//#define PR_DEBUG(fmt, args...)  if (PRINT_PRIORITY <= 0) cli_printf("D [%-10s:%4d] " fmt "\r\n", S_MODULE_NAME, __LINE__, ##args )
//#define PR_INFO(fmt, args...)   if (PRINT_PRIORITY <= 1) cli_printf("%sI [%-10s:%4d]%s " fmt "\r\n", ESC_YELLOW, S_MODULE_NAME, __LINE__, ESC_WHITE, ##args )
//#define PR_WARN(fmt, args...)   if (PRINT_PRIORITY <= 2)  cli_printf("%sW [%-10s:%4d]%s " fmt "\r\n", ESC_GREEN, S_MODULE_NAME, __LINE__, ESC_WHITE, ##args )
//#define PR_ERR(fmt, args...)    if (PRINT_PRIORITY <= 3) cli_printf("%sE [%-10s:%4d]%s " fmt "\r\n", ESC_RED, S_MODULE_NAME, __LINE__, ESC_WHITE, ##args )
//
//#else
//#define PR_DEBUG(fmt, args...)
//#define PR_INFO(fmt, args...)
//#define PR_WARN(fmt, args...)
//#define PR_ERR(fmt, args...)
//#endif	// PRINT_DEBUG


#endif /* APP_SCENARIO_APP_I2C_SLAVE_TEST_PRINT_X_H_ */
