/*
 * task1.h
 *
 *  Created on: 12 Aug 2024
 *      Author: charl
 */

#ifndef APP_WW_PROJECTS_WW_TEMPLATE_TASK1_H_
#define APP_WW_PROJECTS_WW_TEMPLATE_TASK1_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#include "queue.h"
#include "app_msg.h"

/* Task priorities. */
#define hello_task1_PRIORITY	(configMAX_PRIORITIES - 1)

#define TASK1MAGICNUMBER		(0xabcd)

// The states for the task1
typedef enum {
	APP_TASK1_STATE_UNINIT						=0x0000,
	APP_TASK1_STATE_INIT						=0x0001,
	APP_TASK1_STATE_2							=0x0002,
	APP_TASK1_STATE_3							=0x0003,
	APP_TASK1_STATE_ERROR						,
} APP_TASK1_STATE_E;

// Declare strings for each of these states. Values are in task1.c
//extern const char* task1StateString[APP_TASK1_STATE_ERROR + 1];

TaskHandle_t task1_createTask1Task(int8_t priority);

uint16_t task1_getState(void);

const char * task1_getStateString(void);

#endif /* APP_WW_PROJECTS_WW_TEMPLATE_TASK1_H_ */
