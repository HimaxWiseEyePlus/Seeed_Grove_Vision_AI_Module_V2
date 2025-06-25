/*
 * timer_task.h
 *
 *  Created on: 12 Aug 2024
 *      Author: CGP
 *
 * This is an optional simple task that does something regularly
 * such as print the time.
 */

#ifndef APP_WW_PROJECTS_WW500_MD_TIMER_TASK_H_
#define APP_WW_PROJECTS_WW500_MD_TIMER_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#include "queue.h"
#include "ww500_md.h"


// The states for the if_task
// Must match the ifTaskStateString[] array in timer_task.c
// APP_TIMER_NUMSTATES is only used to establish the number of states
typedef enum {
	APP_TIMER_STATE_UNINIT						=0x0000,	// Uninitialised - before task starts
	APP_TIMER_STATE_IDLE						=0x0001,	// While nothing else is happening
	APP_TIMER_NUMSTATES							=0x0002,	// only used to establish the number of states
} APP_TIMER_STATE_E;


// Declare strings for each of these states. Values are in if_task.c
//extern const char* ifTaskStateString[APP_IF_STATE_ERROR + 1];

// Create the task and all its support pieces
TaskHandle_t timerTask_createTask(int8_t priority, uint8_t wakeReason);

// Return the internal state (as a number)
uint16_t timerTask_getState(void);

// Return the internal state (as a string)
const char * timerTask_getStateString(void);


#endif /* APP_WW_PROJECTS_WW500_MD_TIMER_TASK_H_ */
