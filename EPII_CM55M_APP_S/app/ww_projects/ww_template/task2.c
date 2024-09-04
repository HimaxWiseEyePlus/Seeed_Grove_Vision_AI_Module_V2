/*
 * task2.c
 *
 *  Created on: 12 Aug 2024
 *      Author: CGP
 *
 * FreeRTOS task
 *
 */


/*************************************** Includes *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#include "printf_x.h"
#include "xprintf.h"

// FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "task1.h"
#include "task2.h"
#include "app_msg.h"
#include "ww_template.h"

/*************************************** Definitions *******************************************/


#define task2_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define TASK2_QUEUE_LEN   		10

/*************************************** Local Function Declarations *****************************/

static void vTask2Task(void *pvParameters);

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char* ifTaskStateString[APP_TASK2_STATE_ERROR + 1] = {
		"Uninitialised",
		"Initialised",
		"State 2",
		"State 3",
		"State 4",
		"Error"
};

// Strings for expected messages. Values must match Messages directed to Task 2 in app_msg.h
const char* task2EventString[3] = {
		"Task 2 Msg0",
		"Task 2 Msg1",
		"Task 2 Msg2"
};

/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t task2_task_id;

QueueHandle_t     xTask2Queue;

volatile APP_TASK2_STATE_E task2_state = APP_TASK2_STATE_UNINIT;

/*************************************** Local Function Definitions *****************************/


/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void vTask2Task(void *pvParameters) {
    APP_MSG_T rxMessage;
    //APP_MSG_T task2_send_msg;
	APP_TASK2_STATE_E old_state;
	const char * eventString;
	APP_MSG_EVENT_E event;
	uint32_t rxData;

	// One-off initialisation here...

	task2_state = APP_TASK2_STATE_INIT;

	// The task loops forever here, waiting for messages to arrive in its input queue
    for (;;)  {
    	if (xQueueReceive ( xTask2Queue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {

    		// convert event to a string
			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

    		if ((event >= APP_MSG_TASK2_MSG0) && (event <= APP_MSG_TASK2_MSG2)) {
    			eventString = task2EventString[event - APP_MSG_TASK2_MSG0];
    		}
    		else {
    			eventString = "Unexpected";
    		}

    		XP_GREEN;
    		xprintf("\nTask2");
    		XP_WHITE;
			xprintf(" received event '%s' (0x%04x). Value = 0x%08x\r\n", eventString, event, rxData);\

    		old_state = task2_state;

			// For now, just use the events to force a change of state
    		switch(event)  {

			case APP_MSG_TASK2_MSG0:
				task2_state = APP_TASK2_STATE_INIT;
				// special case:
				if (rxData == TASK1MAGICNUMBER) {
		    		xprintf("This message probably came from Task1!\n");
				}
				break;

			case APP_MSG_TASK2_MSG1:
				task2_state = APP_TASK2_STATE_2;
				break;

			case APP_MSG_TASK2_MSG2:
				task2_state = APP_TASK2_STATE_3;
				break;

    		default:
    			task2_state = APP_TASK2_STATE_ERROR;
    			break;
            }

    		if (old_state != task2_state) {
    			// state has changed
        		XP_GREEN;
        		xprintf("Task 2 state changed ");
        		XP_WHITE;
        		xprintf("from '%s' (%d) to '%s' (%d)\r\n",
        				ifTaskStateString[old_state], old_state,
						ifTaskStateString[task2_state], task2_state);
    		}
        }
	}	// for(;;)
}

/*************************************** Exported Function Definitions *****************************/

/**
 * Called by main() to create this task and anything that it needs.
 *
 * The main() code will call vTaskStartScheduler(); to begin FreeRTOS scheduler
 */
void task2_createTask2Task(void) {

	xTask2Queue = xQueueCreate( TASK2_QUEUE_LEN, sizeof(APP_MSG_T) );

	if(xTask2Queue == 0) {
        xprintf("xTask2Queue creation failed!.\r\n");

		configASSERT(0);	// TODO add debug messages?
	}

	if (xTaskCreate(vTask2Task, (const char *)"Task2",
			configMINIMAL_STACK_SIZE,
			NULL, task2_PRIORITY,
			&task2_task_id) != pdPASS)  {
        xprintf("vTask2Task creation failed!.\r\n");

		configASSERT(0);	// TODO add debug messages?
	}
}

/**
 * Returns the internal state as a number
 */
uint16_t task2_getTask2State(void) {
	return task2_state;
}

/**
 * Returns the internal state as a string
 */
const char * task2_getTask2StateString(void) {
	return * &ifTaskStateString[task2_state];
}

