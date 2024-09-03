/*
 * task1.c
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
#include "app_msg.h"
#include "ww130_cli.h"

/*************************************** Definitions *******************************************/


#define task1_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define TASK1_QUEUE_LEN   		10

/*************************************** Local Function Declarations *****************************/

// This is the FreeRTOS task
static void vTask1Task(void *pvParameters);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T  handleEventForInit(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForState2(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForState3(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForError(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T  flagUnexpectedEvent(APP_MSG_T rxMessage);

/*************************************** Local variables *******************************************/

// This is the handle of the task
TaskHandle_t task1_task_id;

QueueHandle_t     xTask1Queue;

// These are the handles for the input queues of Task2. So we can send it messages
extern QueueHandle_t     xIfTaskQueue;

volatile APP_TASK1_STATE_E task1_state = APP_TASK1_STATE_UNINIT;

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char* task1StateString[APP_TASK1_STATE_ERROR + 1] = {
		"Uninitialised",
		"Initialised",
		"State 2",
		"State 3",
		"Error"
};

// Strings for expected messages. Values must match Messages directed to Task 2 in app_msg.h
const char* task1EventString[3] = {
		"Task 1 Msg0",
		"Task 1 Msg1",
		"Task 1 Msg2"
};

/*************************************** Local Function Definitions *****************************/

/**
 * Implements state machine when in APP_TASK1_STATE_INIT
 *
 * Let's say that in APP_TASK1_STATE_INIT we expect only APP_MSG_TASK1_MSG0 and use it to switch to APP_TASK1_STATE_2
 * And that any other event moves us to APP_TASK1_STATE_ERROR
 */
static APP_MSG_DEST_T handleEventForInit(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_TASK1_MSG0:
		//
		task1_state = APP_TASK1_STATE_2;
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		task1_state = APP_TASK1_STATE_ERROR;
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_TASK1_STATE_2
 *
 * Let's say that in APP_TASK1_STATE_2 we expect only APP_MSG_TASK1_MSG1 and use it to switch to APP_TASK1_STATE_3
 * And that any other event moves us to APP_TASK1_STATE_ERROR
 */
static APP_MSG_DEST_T handleEventForState2(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_TASK1_MSG1:
		task1_state = APP_TASK1_STATE_3;
		// We also invent a message to send to Task2, just to show it is possible
		sendMsg.message.msg_data = TASK1MAGICNUMBER;	// send a magic number
		sendMsg.message.msg_event = APP_MSG_TASK2_MSG0;
		sendMsg.destination = xIfTaskQueue;
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		task1_state = APP_TASK1_STATE_ERROR;
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_TASK1_STATE_3
 *
 * Let's say that in APP_TASK1_STATE_3 we expect only APP_MSG_TASK1_MSG2 and use it to switch to APP_TASK1_STATE_INIT
 * And that any other event moves us to APP_TASK1_STATE_ERROR
 */
static APP_MSG_DEST_T handleEventForState3(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_TASK1_MSG2:
		task1_state = APP_TASK1_STATE_INIT;
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		task1_state = APP_TASK1_STATE_ERROR;
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_TASK1_STATE_ERROR
 *
 * Let's say that in APP_TASK1_STATE_ERROR we expect all of the events and use them to switch to another state
 */
static APP_MSG_DEST_T handleEventForError(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_TASK1_MSG0:
		task1_state = APP_TASK1_STATE_2;
		break;

	case APP_MSG_TASK1_MSG1:
		task1_state = APP_TASK1_STATE_3;
		break;

	case APP_MSG_TASK1_MSG2:
		task1_state = APP_TASK1_STATE_INIT;
		break;

	default:
		// here for events that are not expected in this state (actually, there are no others...)
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * For state machine: Print a red message to see if there are unhandled events we should manage
 */
static APP_MSG_DEST_T flagUnexpectedEvent(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;

	XP_LT_RED;
	if ((event >= APP_MSG_TASK1_MSG0) && (event <= APP_MSG_TASK1_MSG2)) {
		xprintf("UNHANDLED event '%s' in '%s'\r\n", task1EventString[event - APP_MSG_TASK1_MSG0], task1StateString[task1_state]);
	}
	else {
		xprintf("UNHANDLED event 0x%04x in '%s'\r\n", event, task1StateString[task1_state]);
	}
	XP_WHITE;

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * This is the FreeRTOS task.
 *
 * It waits for a message from elsewhere, then processes the message according to the current state.
 */
static void vTask1Task(void *pvParameters) {
    APP_MSG_T       rxMessage;
    APP_MSG_DEST_T  txMessage;
	QueueHandle_t   targetQueue;
    APP_MSG_T 		send_msg;

	APP_TASK1_STATE_E old_state;
	const char * eventString;
	APP_MSG_EVENT_E event;
	uint32_t rxData;

	// One-off initialisation here...

	task1_state = APP_TASK1_STATE_INIT;

	// The task loops forever here, waiting for messages to arrive in its input queue
	for (;;)  {
		if (xQueueReceive ( xTask1Queue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {
			// convert event to a string
			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

			if ((event >= APP_MSG_TASK1_MSG0) && (event <= APP_MSG_TASK1_MSG2)) {
				eventString = task1EventString[event - APP_MSG_TASK1_MSG0];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nTask1");
			XP_WHITE;
			xprintf(" received event '%s' (0x%04x). Value = 0x%08x\r\n", eventString, event, rxData);

			old_state = task1_state;

    		// switch on state - and call individual event handling functions
    		switch (task1_state) {

    		case APP_TASK1_STATE_UNINIT:
    			txMessage = flagUnexpectedEvent(rxMessage);
    			break;

    		case APP_TASK1_STATE_INIT:
    			txMessage = handleEventForInit(rxMessage);
    			break;

    		case APP_TASK1_STATE_2:
    			txMessage = handleEventForState2(rxMessage);
    			break;

    		case APP_TASK1_STATE_3:
    			txMessage = handleEventForState3(rxMessage);
    			break;

    		case APP_TASK1_STATE_ERROR:
    			txMessage = handleEventForError(rxMessage);
    			break;

    		default:
    			// should not happen
    			txMessage = flagUnexpectedEvent(rxMessage);
    			break;
    		}

    		if (old_state != task1_state) {
    			// state has changed
        		XP_LT_CYAN;
        		xprintf("Task 1 state changed ");
        		XP_WHITE;
        		xprintf("from '%s' (%d) to '%s' (%d)\r\n",
        				task1StateString[old_state], old_state,
						task1StateString[task1_state], task1_state);
    		}

    		// The processing functions might want us to send a message to another task
    		if (txMessage.destination == NULL) {
				xprintf("No outgoing messages.\n");
    		}
    		else {
    			send_msg = txMessage.message;
    			targetQueue = txMessage.destination;

    			if(xQueueSend( targetQueue , (void *) &send_msg , __QueueSendTicksToWait) != pdTRUE) {
    				xprintf("send send_msg=0x%x fail\r\n", send_msg.msg_event);
    			}
    			else {
    				xprintf("Sending event 0x%04x. Value = 0x%08x\r\n", send_msg.msg_event, send_msg.msg_data);
    			}

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
void task1_createTask1Task(void) {

	xTask1Queue = xQueueCreate( TASK1_QUEUE_LEN, sizeof(APP_MSG_T) );

	if(xTask1Queue == 0) {
        xprintf("xTask1Queue creation failed!.\r\n");

		configASSERT(0);	// TODO add debug messages?
	}

	if (xTaskCreate(vTask1Task, (const char *)"Task1",
			configMINIMAL_STACK_SIZE,
			NULL, task1_task_PRIORITY,
			&task1_task_id) != pdPASS)  {
        xprintf("vTask1Task creation failed!.\r\n");

		configASSERT(0);	// TODO add debug messages?
	}

}

/**
 * Returns the internal state as a number
 */
uint16_t task1_getTask1State(void) {
	return task1_state;
}

/**
 * Returns the internal state as a string
 */
const char * task1_getTask1StateString(void) {
	return * &task1StateString[task1_state];
}



