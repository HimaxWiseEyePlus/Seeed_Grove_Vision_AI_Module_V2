/**************************************************************************************************
 (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
 ------------------------------------------------------------------------
 File        : cli_loop.c
 Project     : WEI
 DATE        : 2013/03/01
 AUTHOR      : 300212
 BRIFE       : This file keep waiting for new command.
 HISTORY     : Initial version - 2013/03/01 created by 300212
 **************************************************************************************************/
#include <string.h>
#include "cli.h"
#include "cli_console.h"
#include "hx_drv_timer.h"
#ifdef IP_timer
#include "timer_interface.h"
#endif
#include "WE2_device.h"

/*******************************************************************************************
 * @brief Loop to listening the incoming command event
 *
 * @param void.
 *
 * @return  void.
 *******************************************************************************************/
void CLI_CommandLoop(void) {
	Console_InitTerminal();

	while (1) {
		while (1) {
			char buf[128];
			char prompt[64];

			CLI_GetCurrentPath(prompt, sizeof(prompt) - 2);
			strcat(prompt, "> ");

			if (readline(buf, sizeof(buf), prompt) == 0)
				continue;
			if (!strcmp("exit", buf))
				break;
			if (!strcmp("bye", buf))
				goto cli_bye;
			if (!strncmp("exec ", buf, 5)) {
				CLI_Execute(buf + 5);
				break;
			}

			CLI_Execute(buf);
		}
		while (keyin() != 23) // Ctrl-W
		{
			// HMX@JACKY : Porting API - cli_mSleep
			//mSleep(200);
	   		hx_drv_timer_cm55x_delay_ms(200, TIMER_STATE_DC);

		}
	}

	cli_bye: Console_RestoreTerminal();
}
