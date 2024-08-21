/*
 * task2.h
 *
 *  Created on: 12 Aug 2024
 *      Author: charl
 */

#ifndef APP_WW_PROJECTS_WW_TEMPLATE_TASK2_H_
#define APP_WW_PROJECTS_WW_TEMPLATE_TASK2_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#include "queue.h"

/* Task priorities. */
#define task2_PRIORITY	(configMAX_PRIORITIES - 2)

// The states for the task2
typedef enum {
	APP_TASK2_STATE_UNINIT						=0x0000,
	APP_TASK2_STATE_INIT						=0x0001,
	APP_TASK2_STATE_2							=0x0002,
	APP_TASK2_STATE_3							=0x0003,
	APP_TASK2_STATE_4							=0x0004,
	APP_TASK2_STATE_ERROR						,
} APP_TASK2_STATE_E;

// Declare strings for each of these states. Values are in task1.c
extern const char* ifTaskStateString[APP_TASK2_STATE_ERROR + 1];


void task2_createTask2Task(void);

uint16_t task2_getTask2State(void);

const char * task2_getTask2StateString(void);


#endif /* APP_WW_PROJECTS_WW_TEMPLATE_TASK2_H_ */
