/**
 * CLI-commands.c
 *
 * Modified by CGP
 *
 * See CLI-commands.h
 */

/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

 ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
 ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

 ***************************************************************************
 *                                                                       *
 *    FreeRTOS provides completely free yet professionally developed,    *
 *    robust, strictly quality controlled, supported, and cross          *
 *    platform software that is more than just the market leader, it     *
 *    is the industry's de facto standard.                               *
 *                                                                       *
 *    Help yourself get started quickly while simultaneously helping     *
 *    to support the FreeRTOS project by purchasing a FreeRTOS           *
 *    tutorial book, reference manual, or both:                          *
 *    http://www.FreeRTOS.org/Documentation                              *
 *                                                                       *
 ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
 */


/*************************************** Includes *******************************************/

/* Modified by Maxim Integrated 26-Jun-2015 to quiet compiler warnings */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

// Himax board UART driver
#include "hx_drv_uart.h"

// Other includes required by the individual CLI commands here:

#include "xprintf.h"
#include "printf_x.h"

#include "task1.h"
#include "app_msg.h"
#include "if_task.h"
#include "CLI-commands.h"
#include "ww130_cli.h"

/*************************************** Definitions *******************************************/

#define USEQUEUE

/* Array sizes */

#define CLI_TASK_QUEUE_LEN   		10

#define NUMRXCHARACTERS 1

/*************************************** External variables *******************************************/

// These are the handles for the input queues of the two tasks. So we can send them messages
extern QueueHandle_t     xTask1Queue;
extern QueueHandle_t     xIfTaskQueue;


/*************************************** Local variables *******************************************/

// Task ID
// This is the handle of the CmdLineTask and is used by the UART callback vCmdLineTask_cb()
// to wake up the task, when a character arrives

TaskHandle_t 	cli_task_id;
QueueHandle_t   xCliTaskQueue;

// Strings for expected messages. Values must match Messages directed to Task 2 in app_msg.h
const char* cliTaskEventString[2] = {
		"Console Char",
		"I2C String"
};


// TODO delete this soon! just to test the verbose command!
static bool verbose;


static bool enabled = false;

/*************************************** Local routine prototypes  *************************************/

// The task code.
static void vCmdLineTask(void *pvParameters);

// UART ISR callback.
static void vCmdLineTask_cb(void);

// Register the CLI commands.
static void vRegisterCLICommands( void );

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Defines a command that returns a table showing the
 * WW-defined state
 */
static BaseType_t prvTaskStateCmd( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/* Enable or disable verbose operation */
static BaseType_t prvVerbose( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

// Force an assert and backtrace
static BaseType_t prvAssert( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

// Force a reset
static BaseType_t prvReset( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvThreeParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

// Gets an event number to send to Task 1
static BaseType_t prvTask1( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

// Report of some status
static BaseType_t prvStatus( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

// Pulse PA0
static BaseType_t prvInt( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvEnable( char *pcWriteBuffer, size_t xWriteBufferLen, const char *  pcCommandString );
static BaseType_t prvDisable( char *pcWriteBuffer, size_t xWriteBufferLen, const char *  pcCommandString );

static void processSingleCharacter(char rxChar);
static char * processString(char * rxString);
static bool startsWith(char *a, const char *b);

/********************************** Structures that define CLI commands  *************************************/

/* Structure that defines the "ps" command line command. */
static const CLI_Command_Definition_t xTaskStats = {
		"ps", /* The command string to type. */
		"ps:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
		prvTaskStatsCommand, /* The function to run. */
		0 /* No parameters are expected. */
};


/* Structure that defines the "state" command line command. */
static const CLI_Command_Definition_t xTaskState = {
		"states", /* The command string to type. */
		"states:\r\n Displays a table showing the internal states of WW tasks\r\n",
		prvTaskStateCmd, /* The function to run. */
		0 /* No parameters are expected. */
};


/* Structure that defines the "verbose" command line command. */
static const CLI_Command_Definition_t xVerbose = {
		"verbose", /* The command string to type. */
		"verbose <0/1>:\r\n Disable (0) or enable (1) tick-tock messages\r\n",
		prvVerbose, /* The function to run. */
		1 /* One parameter expected */
};

/* Structure that defines the "assert" command line command. */
static const CLI_Command_Definition_t xAssert = {
		"assert", /* The command string to type. */
		"assert:\r\n Forces an assert error\r\n",
		prvAssert, /* The function to run. */
		0 /* No parameters are expected. */
};

/* Structure that defines the "reset" command line command. */
static const CLI_Command_Definition_t xReset = {
		"reset", /* The command string to type. */
		"reset:\r\n Forces an reset\r\n",
		prvReset, /* The function to run. */
		0 /* No parameters are expected. */
};

/* Structure that defines the "echo_3_parameters" command line command.  This
takes exactly three parameters that the command simply echos back one at a
time. */
static const CLI_Command_Definition_t xThreeParameterEcho = {
		"echo-3-parameters",
		"echo-3-parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n",
		prvThreeParameterEchoCommand, /* The function to run. */
		3 /* Three parameters are expected, which can take any value. */
};

/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
static const CLI_Command_Definition_t xParameterEcho = {
		"echo-parameters",
		"echo-parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n",
		prvParameterEchoCommand, /* The function to run. */
		-1 /* The user can enter any number of commands. */
};


/* Structure that defines the "task1" command line command. */
static const CLI_Command_Definition_t xTask1 = {
		"task1", /* The command string to type. */
		"task1 <event>:\r\n Send <event> (a number) to Task 1 \r\n",
		prvTask1, /* The function to run. */
		1 /* One parameter expected */
};

/* Structure that defines the "status" command line command. */
static const CLI_Command_Definition_t xStatus = {
		"status", /* The command string to type. */
		"status:\r\n Send a status report\r\n",
		prvStatus, /* The function to run. */
		0 /* No parameters expected */
};

/* Structure that defines the "enable" command line command. */
static const CLI_Command_Definition_t xEnable = {
		"enable", /* The command string to type. */
		"enable:\r\n Enable (something)\r\n",
		prvEnable, /* The function to run. */
		0 /* No parameters expected */
};

/* Structure that defines the "disable" command line command. */
static const CLI_Command_Definition_t xDisable = {
		"disable", /* The command string to type. */
		"disable:\r\n Disable (something)\r\n",
		prvDisable, /* The function to run. */
		0 /* No parameters expected */
};


/* Structure that defines the "verbose" command line command. */
static const CLI_Command_Definition_t xInt = {
		"int", /* The command string to type. */
		"int <pulsewidth>:\r\n Pulse PA0 for <pulsewidth>ms\r\n",
		prvInt, /* The function to run. */
		1 /* One parameter expected */
};

/********************************** Private Functions - for CLI commands *************************************/

// One of these commands for each activity invoked by the CLI

// Print the task list and some stats
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *const pcHeader = "Task          State  Priority  Stack	#\r\n************************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, pcHeader );
	vTaskList( pcWriteBuffer + strlen( pcHeader ) );

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}


// Displays a table showing the internal states of WW tasks
// This function has hard-coded calls to functions within the tasks themselves,
// So must be updated to reflect the actual tasks and functions in use.
static BaseType_t prvTaskStateCmd( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *const pcHeader = "Task  State#    State Name\r\n************************************************\r\n";
	uint16_t stateValue;
	const char * stateString;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	// Generate a table of internal task states.
	strcpy( pcWriteBuffer, pcHeader );
	pcWriteBuffer += strlen( pcWriteBuffer );

	// Functions and strings are hard-coded here!
	stateValue = task1_getTask1State();
	stateString = task1_getTask1StateString();
	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Task1     %d     %s\r\n", stateValue, stateString);

	stateValue = ifTask_getTaskState();
	stateString = ifTask_getTaskStateString();
	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Task2     %d     %s\r\n", stateValue, stateString);

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}
// Set or clear a boolean called verbose (not used at the moment, but could be!
static BaseType_t prvVerbose( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *pcParameter;
	BaseType_t lParameterStringLength;

	/* Get parameter */
	pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	if (pcParameter != NULL) {
		if (pcParameter[0] == '0') {
			verbose = false;
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Verbose is off\r\n");
		} else if (pcParameter[0] == '1') {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Verbose is on\r\n");
			verbose = true;
		} else {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply 0 (Disable) or 1 (Enable)\r\n");
		}
	} else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply 0 (Disable) or 1 (Enable)\r\n");
	}

	return pdFALSE;
}


static BaseType_t prvAssert( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	xprintf("Forcing error:\r\n");
	configASSERT(xWriteBufferLen == 1235);	// call if argument is false. Called if configASSERT is defined

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

// Resets the device
static BaseType_t prvReset( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	XP_RED;
	xprintf("Forcing reset.\r\n");
	XP_WHITE;
	vTaskDelay(pdMS_TO_TICKS(300));

	NVIC_SystemReset();

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

static BaseType_t prvThreeParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *pcParameter;
	BaseType_t xParameterStringLength, xReturn;
	static UBaseType_t uxParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( pcWriteBuffer, "The three parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		uxParameterNumber = 1U;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
				(
						pcCommandString,		/* The command string itself. */
						uxParameterNumber,		/* Return the next parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
				);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		sprintf( pcWriteBuffer, "%d: ", ( int ) uxParameterNumber );
		strncat( pcWriteBuffer, pcParameter, ( size_t ) xParameterStringLength );
		// Changed to stop warning message
		//strncat( pcWriteBuffer, "\r\n", strlen( "\r\n" ) );
		strcat( pcWriteBuffer, "\r\n");

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( uxParameterNumber == 3U )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			xReturn = pdFALSE;
			uxParameterNumber = 0;
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
	}

	return xReturn;
}


static BaseType_t prvParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *pcParameter;
	BaseType_t xParameterStringLength, xReturn;
	static UBaseType_t uxParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( pcWriteBuffer, "The parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		uxParameterNumber = 1U;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
				(
						pcCommandString,		/* The command string itself. */
						uxParameterNumber,		/* Return the next parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
				);

		if( pcParameter != NULL )
		{
			/* Return the parameter string. */
			memset( pcWriteBuffer, 0x00, xWriteBufferLen );
			sprintf( pcWriteBuffer, "%d: ", ( int ) uxParameterNumber );
			strncat( pcWriteBuffer, ( char * ) pcParameter, ( size_t ) xParameterStringLength );
			// Changed to stop warning message
			//strncat( pcWriteBuffer, "\r\n", strlen( "\r\n" ) );
			strcat( pcWriteBuffer, "\r\n");

			/* There might be more parameters to return after this one. */
			xReturn = pdTRUE;
			uxParameterNumber++;
		}
		else
		{
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[ 0 ] = 0x00;

			/* No more data to return. */
			xReturn = pdFALSE;

			/* Start over the next time this command is executed. */
			uxParameterNumber = 0;
		}
	}

	return xReturn;
}

// Sends an event to Task 1
static BaseType_t prvTask1( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *pcParameter;
	BaseType_t lParameterStringLength;
	uint16_t eventNum;
    APP_MSG_T task1_send_msg;

	/* Get parameter */
	pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	if (pcParameter != NULL) {
		eventNum = atoi(pcParameter);
		if (eventNum > 0) {
			task1_send_msg.msg_data = 0;
			task1_send_msg.msg_event = APP_MSG_TASK1_MSG0 + eventNum - 1;
			if(xQueueSend( xTask1Queue , (void *) &task1_send_msg , __QueueSendTicksToWait) != pdTRUE) {
				pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "send task1_send_msg=0x%x fail\r\n", task1_send_msg.msg_event);
			}
			else {
				pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Sending event 0x%04x to Task 1\r\n",
						task1_send_msg.msg_event);
			}
		}
		else {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply an integer > 0\r\n");
		}

	}
	else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply an integer > 0\r\n");
	}

	return pdFALSE;
}

// Reports on some status
static BaseType_t prvStatus( char *pcWriteBuffer, size_t xWriteBufferLen, const char *  pcCommandString ) {

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	sprintf(pcWriteBuffer, "Status: %s", enabled ?"enabled":"disabled");

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

// Sets some state
static BaseType_t prvEnable( char *pcWriteBuffer, size_t xWriteBufferLen, const char *  pcCommandString ) {

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	sprintf(pcWriteBuffer, "Enabled (something)");
	enabled = true;

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

// Sets some state
static BaseType_t prvDisable( char *pcWriteBuffer, size_t xWriteBufferLen, const char *  pcCommandString ) {

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	sprintf(pcWriteBuffer, "Disabled (something)");
	enabled = false;

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

// Pulse the PA0 pin for nms
static BaseType_t prvInt( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	const char *pcParameter;
	BaseType_t lParameterStringLength;
	uint16_t interval;
	APP_MSG_T send_msg;

	/* Get parameter */
	pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	if (pcParameter != NULL) {

		interval = atoi(pcParameter);

		if ((interval > 0) && (interval < 10000)) {
			send_msg.msg_data = interval;
			send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_PA0_INT_OUT;

			if(xQueueSend( xIfTaskQueue , (void *) &send_msg , __QueueSendTicksToWait) != pdTRUE) {
				pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "send 0x%x fail\r\n", send_msg.msg_event);
			}
			else {
				pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Requesting pulse of %dms\r\n", interval);
			}
		}
		else {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply a time > 0ms and < 10000ms\r\n");
		}
	}
	else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply a time in ms\r\n");
	}

	return pdFALSE;
}

/********************************** Private Functions - Other *************************************/

/**
 * UART interrupt callback.
 *
 * Called after a character is received and placed in rxChar.
 * Wakes the waiting command processor task
 *
 * cli_task_id was set by the xTaskCreate()
 */
static void vCmdLineTask_cb(void) {
	BaseType_t xHigherPriorityTaskWoken;

#ifdef USEQUEUE
    APP_MSG_T 		send_msg;
	send_msg.msg_data = 0;	// TODO - put the character here?
	send_msg.msg_event = APP_MSG_CLITASK_RXCHAR;

	xQueueSendFromISR( xCliTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
	if( xHigherPriorityTaskWoken ) {
		taskYIELD();
	}

#else
	xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(cli_task_id, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif	// USEQUEUE
}


/**
 * Process a single character that has arrived from the console UART.
 *
 *
 */
static void processSingleCharacter(char rxChar) {
	static uint16_t index = 0;     					/* Index into cliBuffer */
	static char cliInBuffer[CLI_CMD_LINE_BUF_SIZE];  	/* Buffer for input */
	static char cliOutBuffer[CLI_OUTPUT_BUF_SIZE];      /* Buffer for output */
	BaseType_t xMore;

#ifdef ORIGINAL
	uint16_t i;
	char outChar;
#endif	//ORIGINAL

	switch (rxChar) {
	case (0x08):
		// Backspace
		if (index > 0) {
			index--;
			xprintf("\x08 \x08");
		}
		fflush(stdout);
		break;

	case (0x03):
		//^C abort
		index = 0;
		xprintf("^C");
		XP_YELLOW;
		xprintf("\ncmd> ");
		XP_WHITE;
		break;

	case '\r':
		// Ignore. Take action on '\n' only
		break;

	case '\n': {
		xprintf("\r\n");
		// Null terminate the string in the receive buffer for safety
		cliInBuffer[index] = '\0';

		// Evaluate the command - loop while the registered command returns true.
		// e.g. a "dir" command loops through for every directory entry
		do {
			memset(cliOutBuffer, 0, CLI_OUTPUT_BUF_SIZE);
			xMore = FreeRTOS_CLIProcessCommand(cliInBuffer, cliOutBuffer, CLI_OUTPUT_BUF_SIZE);
			/* If xMore == pdTRUE, then output buffer contains no null termination, so
			 *  we know it is OUTPUT_BUF_SIZE. If pdFALSE, we can use strlen.
			 */
			//						for (x = 0; x < (xMore == pdTRUE ? OUTPUT_BUF_SIZE : strlen(output)) ; x++) {
			//							char outChar = *(output + x);
			//							if (outChar != 0) {
			//								putchar(outChar);
			//							}
			//						}
#ifdef ORIGINAL
			// print the output[] cliBuffer until the first zero
			for (i = 0; i < CLI_OUTPUT_BUF_SIZE; i++) {
				outChar = *(cliOutBuffer + i);
				if (outChar == 0) {
					break;
				}
				putchar(outChar);
			}
#else
			// Surely this is the same at the putchar loop?
			xprintf("%s\n", cliOutBuffer);
#endif // ORIGINAL
		} while (xMore != pdFALSE);

		// New prompt
		index = 0;
		XP_YELLOW;
		xprintf("cmd> ");
		XP_WHITE;
		fflush(stdout);
		break;
	} // '\n\'

	default:
		// 'normal' characters
		if (index < CLI_CMD_LINE_BUF_SIZE) {
			putchar(rxChar);
			cliInBuffer[index++] = rxChar;
			fflush(stdout);
		}
		else {
			// Throw away data and beep terminal
			putchar(0x07);
			fflush(stdout);
		}
		break;

	}	// switch
}


/**
 * String utility
 */
static bool startsWith(char *a, const char *b) {
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

/**
 * Process a whole line
 *
 * Used to process a string received from the WW130 over I2C.
 *
 * An unrecognised command will receive: 'Command not recognised.  Enter 'help' to view a list of available commands.'
 *
 */
static char * processString(char * rxString) {
	static char cliOutBuffer[CLI_OUTPUT_BUF_SIZE];       /* Buffer for output */
	BaseType_t xMore;

	do {
		memset(cliOutBuffer, 0, CLI_OUTPUT_BUF_SIZE);
		xMore = FreeRTOS_CLIProcessCommand(rxString, cliOutBuffer, CLI_OUTPUT_BUF_SIZE);
		xprintf("%s\n", cliOutBuffer);

	} while (xMore != pdFALSE);

	// Truncate the long 'Command not recognised.  Enter 'help' to view a list of available commands.' message
	if (startsWith(cliOutBuffer, "Command not recognised")) {
		strcpy(cliOutBuffer, "Unrecognised");
	}
	return cliOutBuffer;
}

/* =| vCmdLineTask |======================================
 *
 * The command line task provides a prompt on the serial
 *  interface and takes input from the user to evaluate
 *  via the FreeRTOS+CLI parser.
 *
 * NOTE: FreeRTOS+CLI is part of FreeRTOS+ and has
 *  different licensing requirements. Please see
 *  http://www.freertos.org/FreeRTOS-Plus for more information
 *
 *  CGP. Looks like:
 *  (1)		This task enables the interrupt UART interrupt and registers a callback
 *  (2)		It sits in a loop calling MXC_UART_TransactionAsync() which waits for the callback
 *  		to release the task with vTaskNotifyGiveFromISR()
 *  (3)		When released it processes character(s) and calls FreeRTOS_CLIProcessCommand() when appropriate
 *
 * =======================================================
 */
static void vCmdLineTask(void *pvParameters) {
	char rxChar;
	DEV_UART_PTR dev_uart_ptr;
	DEV_BUFFER rx_buffer;
    APP_MSG_T       rxMessage;
    APP_MSG_T       send_msg;
	APP_MSG_EVENT_E event;
	uint32_t rxData;
	//APP_CLITASK_STATE_E old_state;
	char * response;

	/* Register available CLI commands */
	vRegisterCLICommands();

	xprintf("\nEnter 'help' to view a list of available commands.\n");
	XP_YELLOW;
	xprintf("cmd> ");
	XP_WHITE;
	fflush(stdout);

	// Prepare the console UART for interrupt-driven receive of characters into rxChar
	dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_0);
	dev_uart_ptr->uart_open(UART_BAUDRATE_921600);
	dev_uart_ptr->uart_control(UART_CMD_SET_RXCB,  (UART_CTRL_PARAM) vCmdLineTask_cb);

	rx_buffer.buf = (void *) &rxChar;
	rx_buffer.len = NUMRXCHARACTERS;

	// Enable console UART t receive characters, interrupt-driven
	dev_uart_ptr->uart_control(UART_CMD_SET_RXINT_BUF, (UART_CTRL_PARAM) &rx_buffer);
	dev_uart_ptr->uart_control(UART_CMD_SET_RXINT, (UART_CTRL_PARAM) 1);

	for (;;) {
		if (xQueueReceive ( xCliTaskQueue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {
			// convert event to a string
			event = rxMessage.msg_event;
			rxData = rxMessage.msg_data;

#if 0
			const char * eventString;
			if ((event >= APP_MSG_CLITASK_RXCHAR) && (event <= APP_MSG_CLITASK_RXI2C)) {
				eventString = cliTaskEventString[event - APP_MSG_CLITASK_RXCHAR];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nCLI Task ");
			XP_WHITE;
			xprintf("received event '%s' (0x%04x). Value = 0x%08x\r\n", eventString, event, rxData);

			//old_state = task1_state;
#endif
			// For now, switch on event
			switch (event) {
			case APP_MSG_CLITASK_RXCHAR:
				// process the character - calling the CLI command as necessary
				processSingleCharacter(rxChar);

				// Reset the cliBuffer then re-enable the interrupts
				rx_buffer.buf = (void *) &rxChar;
				rx_buffer.len = NUMRXCHARACTERS;

				dev_uart_ptr->uart_control(UART_CMD_SET_RXINT_BUF, (UART_CTRL_PARAM) &rx_buffer);
				dev_uart_ptr->uart_control(UART_CMD_SET_RXINT, (UART_CTRL_PARAM) 1);

				break;

			case APP_MSG_CLITASK_RXI2C:// String has arrived via I2C from WW130
				//xprintf("\nDEBUG: CLI task has received '%s'\n", (char *) rxData);

				response = processString((char *) rxData);
				//xprintf("DEBUG: CLI response '%s'\n", (char *) response);

				send_msg.msg_data = (uint32_t) response;
				send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_CLI_RESPONSE;
				if(xQueueSend( xIfTaskQueue, (void *) &send_msg , __QueueSendTicksToWait) != pdTRUE) {
					xprintf("send_msg=0x%x fail\r\n", send_msg.msg_event);
				}

				break;

			default:

				break;
			} // switch()
		}

	} // for(;;)

}

/**
 * Register CLI commands
 */
static void vRegisterCLICommands( void ) {
	FreeRTOS_CLIRegisterCommand( &xTaskStats );
	FreeRTOS_CLIRegisterCommand( &xTaskState );
	FreeRTOS_CLIRegisterCommand( &xVerbose );
	FreeRTOS_CLIRegisterCommand( &xAssert );
	FreeRTOS_CLIRegisterCommand( &xReset );
	FreeRTOS_CLIRegisterCommand( &xThreeParameterEcho );
	FreeRTOS_CLIRegisterCommand( &xParameterEcho );
	FreeRTOS_CLIRegisterCommand( &xTask1 );
	FreeRTOS_CLIRegisterCommand( &xStatus );
	FreeRTOS_CLIRegisterCommand( &xEnable );
	FreeRTOS_CLIRegisterCommand( &xDisable );
	FreeRTOS_CLIRegisterCommand( &xInt );
}

/********************************** Public Functions  *************************************/

/**
 * Creates the CLI task.
 *
 * The task itself registers the commands and sets up the UART interrupt and ISR callback.
 *
 * Not sure how bug the stack needs to be...
 */
void cli_createCLITask(void) {

	xCliTaskQueue  = xQueueCreate( CLI_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xCliTaskQueue == 0) {
		xprintf("Failed to create xCliTaskQueue\n");
		configASSERT(0);	// TODO add debug messages?
	}

	if (xTaskCreate(vCmdLineTask, (const char *)"CLI",
			3 * configMINIMAL_STACK_SIZE + CLI_CMD_LINE_BUF_SIZE + CLI_OUTPUT_BUF_SIZE,
			NULL, tskIDLE_PRIORITY+1, &cli_task_id) != pdPASS)  {
		xprintf("Failed to create vCmdLineTask\n");
		configASSERT(0);	// TODO add debug messages?
	}
}

