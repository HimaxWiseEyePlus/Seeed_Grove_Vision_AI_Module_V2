/*
 * CLI-commands.h
 *
 *  Created on: 25 Jul 2022
 *      Author: CGP
 *
 * (Based on work done for the MAX78000 in July 2022 and modified for the Himax chip.)
 *
 * The standard CLI-commands.c has no .h file, and only one public function: vRegisterCLICommands()
 *
 * To make it easier to use the CLI functionality in the future, I have brought together other functions necessary
 * for CLI.
 *
 * Collects together all CLI support code: the FreeRTOS task, UART ISR and callback...
 *
 * Now the user calls cli_createCLITask() and that routine sets up the task, and the task calls vRegisterCLICommands()
 *
 * For information on FreeRTOS CLI in general:
 *		https://www.freertos.org/Documentation/03-Libraries/02-FreeRTOS-plus/03-FreeRTOS-plus-CLI/01-FreeRTOS-plus-CLI
 *		https://iotexpert.com/freertos-command-line-interface/
 *
 * To get this working I had to research to to get the console input. Some clues in hx_drv_uart.h
 * I based my work the code in "Usage-4: Receive data using interrput mode with UART 1"	and adjusted
 * for UART 0 and receiving 1 character at a time.
 *
 */

#ifndef CLI_COMMANDS_H_
#define CLI_COMMANDS_H_


#include "task.h"
#include "if_task.h"

#define CLI_CMD_LINE_BUF_SIZE       80
#define CLI_OUTPUT_BUF_SIZE         WW130_MAX_PAYLOAD_SIZE


TaskHandle_t cli_createCLITask(int8_t priority);

uint16_t cli_getState(void);
const char * cli_getStateString(void);

#endif /* CLI_COMMANDS_H_ */
