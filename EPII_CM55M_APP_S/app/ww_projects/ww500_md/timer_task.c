/*
 * timer_task.c
 *
 *  Created on: 12 Aug 2024
 *      Author: CGP
 *
 * FreeRTOS task
 *
 * This is an optional simple task that does something regularly
 * such as print the time.
 *
 */

/*************************************** Includes *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "WE2_core.h"
#include "board.h"

#include "printf_x.h"
#include "xprintf.h"

// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "app_msg.h"
#include "timer_task.h"

#include "ww500_md.h"
#include "exif_utc.h"

/*************************************** Definitions *******************************************/

#define TIMERPERIOD	2000
#define TIMERCOUNT	10
#define TIMERCOUNTCOLD	5

/*************************************** Local Function Declarations *****************************/

static void vTimerTask(void *pvParameters);
static void printTime(void);

/*************************************** Local variables *******************************************/

static APP_WAKE_REASON_E woken;

// This is the handle of the task
TaskHandle_t 	timerTask_task_id;
//QueueHandle_t     xTimerTaskQueue;

volatile APP_IF_STATE_E timer_task_state = APP_TIMER_STATE_UNINIT;

// Strings for each of these states. Values must match APP_TIMER_STATE_E in task1.h
const char * timerTaskStateString[APP_TIMER_NUMSTATES] = {
		"Uninitialised",
		"Idle",
};


/*************************************** Local Function Definitions *****************************/

static void printTime(void) {
	rtc_time time = {0};
	char timeString[UTCSTRINGLENGTH];

	exif_utc_get_rtc_as_time(&time);
	exif_utc_time_to_exif_string(&time, timeString, sizeof(timeString));

	XP_LT_GREEN;
	xprintf(" >>> %s\n", timeString);
	XP_WHITE;
}

/********************************** FreeRTOS Task  *************************************/

/**
 * FreeRTOS task responsible for handling interface with WW130
 */
static void vTimerTask(void *pvParameters) {

	// One-off initialisation here...

    XP_CYAN;
    // Observing these messages confirms the initialisation sequence
    xprintf("Starting TimerTask\n");
    XP_WHITE;

	timer_task_state = APP_TIMER_STATE_IDLE;

    const TickType_t xDelay = pdMS_TO_TICKS(TIMERPERIOD); // TIMERPERIOD in milliseconds

    uint16_t count;

    if (woken == 1) {
    	count = TIMERCOUNTCOLD;
    }
    else {
    	count = TIMERCOUNT;
    }

    while (count > 0)  {
    	printTime();
    	count--;
    	vTaskDelay(xDelay);
    }

    // Cleanly delete the task
    vTaskDelete(NULL);  // NULL means delete this task
}


/*************************************** Exported Function Definitions *****************************/

/**
 * Called by app_main() to create this task and anything that it needs.
 *
 * The app_main() code will call vTaskStartScheduler() to begin FreeRTOS scheduler
 */
TaskHandle_t timerTask_createTask(int8_t priority, APP_WAKE_REASON_E wakeReason) {

    woken = wakeReason;

	if (priority < 0){
		priority = 0;
	}

	if (xTaskCreate(vTimerTask, (const char *)"TimerTask",
			configMINIMAL_STACK_SIZE * 3,
			NULL, priority,
			&timerTask_task_id) != pdPASS)  {
        xprintf("vTimerTask creation failed!.\r\n");
		configASSERT(0);	// TODO add debug messages?
	}

	return timerTask_task_id;
}

/**
 * Returns the internal state as a number
 */
uint16_t timerTask_getState(void) {
	return timer_task_state;
}

/**
 * Returns the internal state as a string
 */
const char * timerTask_getStateString(void) {
	return * &timerTaskStateString[timer_task_state];
}

