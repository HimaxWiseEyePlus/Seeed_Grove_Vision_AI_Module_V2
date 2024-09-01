/*
 * printf_x.h
 *
 *  Created on: 29 Jul 2024
 *      Author: charl
 *
 * Macros to print different colours
 */

#ifndef APP_SCENARIO_APP_ALLON_SENSOR_TFLM_FREERTOS_PRINTF_X_H_
#define APP_SCENARIO_APP_ALLON_SENSOR_TFLM_FREERTOS_PRINTF_X_H_

// See here for colours: https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
// https://wiki.bash-hackers.org/scripting/terminalcodes

#define ESC_NONE "\033[m"
#define ESC_RED "\033[0;32;31m"
#define ESC_LIGHT_RED "\033[1;31m"
#define ESC_GREEN "\033[0;32;32m"
#define ESC_LIGHT_GREEN "\033[1;32m"
#define ESC_BLUE "\033[0;32;34m"
#define ESC_LIGHT_BLUE "\033[1;34m"
#define ESC_DARK_GRAY "\033[1;30m"
#define ESC_CYAN "\033[0;36m"
#define ESC_LIGHT_CYAN "\033[1;36m"
#define ESC_PURPLE "\033[0;35m"
#define ESC_MAGENTA "\033[1;35m"
#define ESC_BROWN "\033[0;33m"
#define ESC_YELLOW "\033[1;33m"
#define ESC_LIGHT_GRAY "\033[0;37m"
#define ESC_WHITE "\033[1;37m"

#define XP_NONE		xprintf("%s", ESC_NONE);
#define XP_RED		xprintf("%s", ESC_RED);
#define XP_LT_RED	xprintf("%s", ESC_LIGHT_RED);
#define XP_GREEN	xprintf("%s", ESC_GREEN);
#define XP_LT_GREEN	xprintf("%s", ESC_LIGHT_GREEN);
#define XP_BROWN	xprintf("%s", ESC_BROWN);
#define XP_YELLOW	xprintf("%s", ESC_YELLOW);
#define XP_BLUE		xprintf("%s", ESC_BLUE);
#define XP_LT_BLUE	xprintf("%s", ESC_LIGHT_BLUE);
#define XP_PURPLE	xprintf("%s", ESC_PURPLE);
#define XP_MAGENTA	xprintf("%s", ESC_MAGENTA);
#define XP_CYAN		xprintf("%s", ESC_CYAN);
#define XP_LT_CYAN	xprintf("%s", ESC_LIGHT_CYAN);
#define XP_GREY		xprintf("%s", ESC_DARK_GRAY);	// Looks like this is black
#define XP_LT_GREY	xprintf("%s", ESC_LIGHT_GRAY);
#define XP_WHITE	xprintf("%s", ESC_WHITE);

// print test string in each colour
void printf_x_test(void);

#endif /* APP_SCENARIO_APP_ALLON_SENSOR_TFLM_FREERTOS_PRINTF_X_H_ */
