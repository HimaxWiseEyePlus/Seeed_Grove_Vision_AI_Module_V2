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
#include "task2.h"
#include "app_msg.h"
#include "ww_template.h"

/*************************************** Definitions *******************************************/

/* Array sizes */
#define CLI_CMD_LINE_BUF_SIZE       80
#define CLI_OUTPUT_BUF_SIZE         512

#define NUMRXCHARACTERS 1


#ifndef  configINCLUDE_TRACE_RELATED_CLI_COMMANDS
// Looks like this relates to "collecting trace data" and requires extra code to be installed.
// https://www.freertos.org/Documentation/03-Libraries/02-FreeRTOS-plus/FreeRTOS-Plus/FreeRTOS_Plus_Trace/RTOS_Trace_Instructions
#define configINCLUDE_TRACE_RELATED_CLI_COMMANDS 0
#endif

#ifndef configINCLUDE_QUERY_HEAP_COMMAND
#define configINCLUDE_QUERY_HEAP_COMMAND 1
#endif

/*************************************** External variables *******************************************/

// TODO delete this soon! just to test the verbose command!
static bool verbose;

// These are the handles for the input queues of the two tasks. So we can send them messages
extern QueueHandle_t     xTask1Queue;
extern QueueHandle_t     xTask2Queue;

/*************************************** Local variables *******************************************/

// Task ID
// This is the handle of the CmdLineTask and is used by the UART callback vCmdLineTask_cb()
// to wake up the task, when a character arrives

TaskHandle_t cli_task_id;

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

// Gets an event number and a value to send to Task 2
static BaseType_t prvTask2( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );


/*
 * Implements the run-time-stats command.
 */
#if( configGENERATE_RUN_TIME_STATS == 1 )
static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif /* configGENERATE_RUN_TIME_STATS */

/*
 * Implements the "query heap" command.
 */
#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
static BaseType_t prvQueryHeapCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif

/*
 * Implements the "trace start" and "trace stop" commands;
 */
#if( configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1 )
static BaseType_t prvStartStopTraceCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif


/********************************** Structures that define CLI commands  *************************************/

/* Structure that defines the "ps" command line command. */
static const CLI_Command_Definition_t xTaskStats = {
		"ps", /* The command string to type. */
		"\r\nps:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
		prvTaskStatsCommand, /* The function to run. */
		0 /* No parameters are expected. */
};


/* Structure that defines the "state" command line command. */
static const CLI_Command_Definition_t xTaskState = {
		"states", /* The command string to type. */
		"\r\nstates:\r\n Displays a table showing the internal states of WW tasks\r\n",
		prvTaskStateCmd, /* The function to run. */
		0 /* No parameters are expected. */
};


/* Structure that defines the "verbose" command line command. */
static const CLI_Command_Definition_t xVerbose = {
		"verbose", /* The command string to type. */
		"\r\nverbose <0/1>:\r\n Disable (0) or enable (1) tick-tock messages\r\n",
		prvVerbose, /* The function to run. */
		1 /* One parameter expected */
};

/* Structure that defines the "assert" command line command. */
static const CLI_Command_Definition_t xAssert = {
		"assert", /* The command string to type. */
		"\r\nassert:\r\n Forces an assert error\r\n",
		prvAssert, /* The function to run. */
		0 /* No parameters are expected. */
};

/* Structure that defines the "reset" command line command. */
static const CLI_Command_Definition_t xReset = {
		"reset", /* The command string to type. */
		"\r\nreset:\r\n Forces an reset\r\n",
		prvReset, /* The function to run. */
		0 /* No parameters are expected. */
};

/* Structure that defines the "echo_3_parameters" command line command.  This
takes exactly three parameters that the command simply echos back one at a
time. */
static const CLI_Command_Definition_t xThreeParameterEcho = {
		"echo-3-parameters",
		"\r\necho-3-parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n",
		prvThreeParameterEchoCommand, /* The function to run. */
		3 /* Three parameters are expected, which can take any value. */
};

/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
static const CLI_Command_Definition_t xParameterEcho = {
		"echo-parameters",
		"\r\necho-parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n",
		prvParameterEchoCommand, /* The function to run. */
		-1 /* The user can enter any number of commands. */
};


/* Structure that defines the "task1" command line command. */
static const CLI_Command_Definition_t xTask1 = {
		"task1", /* The command string to type. */
		"\r\ntask1 <event>:\r\n Send <event> (a number) to Task 1 \r\n",
		prvTask1, /* The function to run. */
		1 /* One parameter expected */
};

/* Structure that defines the "task2" command line command. */
static const CLI_Command_Definition_t xTask2 = {
		"task2", /* The command string to type. */
		"\r\ntask2 <event> <value>:\r\n Send <event> (a number) and <value> to Task 2 \r\n",
		prvTask2, /* The function to run. */
		2 /* No parameters are expected. */
};


#if( configGENERATE_RUN_TIME_STATS == 1 )
/* Structure that defines the "run-time-stats" command line command.   This
	generates a table that shows how much run time each task has */
static const CLI_Command_Definition_t xRunTimeStats =
{
		"run-time-stats", /* The command string to type. */
		"\r\nrun-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n",
		prvRunTimeStatsCommand, /* The function to run. */
		0 /* No parameters are expected. */
};
#endif /* configGENERATE_RUN_TIME_STATS */


#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
/* Structure that defines the "query_heap" command line command. */
static const CLI_Command_Definition_t xQueryHeap =
{
		"query-heap",
		"\r\nquery-heap:\r\n Displays the free heap space, and minimum ever free heap space.\r\n",
		prvQueryHeapCommand, /* The function to run. */
		0 /* The user can enter any number of commands. */
};
#endif /* configQUERY_HEAP_COMMAND */

#if configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1
/* Structure that defines the "trace" command line command.  This takes a single
	parameter, which can be either "start" or "stop". */
static const CLI_Command_Definition_t xStartStopTrace =
{
		"trace",
		"\r\ntrace [start | stop]:\r\n Starts or stops a trace recording for viewing in FreeRTOS+Trace\r\n",
		prvStartStopTraceCommand, /* The function to run. */
		1 /* One parameter is expected.  Valid values are "start" and "stop". */
};
#endif /* configINCLUDE_TRACE_RELATED_CLI_COMMANDS */

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
	// TODO fxme
	stateValue = task1_getState();
	stateString = task1_getStateString();
	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Task1     %d     %s\r\n", stateValue, stateString);

	stateValue = task2_getTask2State();
	stateString = task2_getTask2StateString();
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
// Sends an event plus data to Task 2
static BaseType_t prvTask2( char *pcWriteBuffer, size_t xWriteBufferLen, const char *  pcCommandString ) {
	int param1;
	int param2;
	const char *pcParameter1;
	const char *pcParameter2;
	BaseType_t xParameter1StringLength;
	BaseType_t xParameter2StringLength;
    APP_MSG_T task2_send_msg;

	pcParameter1 = FreeRTOS_CLIGetParameter( pcCommandString, 1, &xParameter1StringLength);
	pcParameter2 = FreeRTOS_CLIGetParameter( pcCommandString, 2, &xParameter2StringLength);

	if ((pcParameter1 != NULL) && (pcParameter1 != NULL)) {
		// Parse the parameters from the command string using atoi()
		param1 = atoi(pcParameter1);
		param2 = atoi(pcParameter2);

		// Check if the parsed values are valid (greater than 0)
		if (param1 <= 0 || param2 <= 0)    {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Both parameters must be positive integers.\r\n");
		}
		else {
			task2_send_msg.msg_data = param2;
			task2_send_msg.msg_event = APP_MSG_TASK2_MSG0 + param1 - 1;
			if(xQueueSend( xTask2Queue , (void *) &task2_send_msg , __QueueSendTicksToWait) != pdTRUE) {
				pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "send task2_send_msg = 0x%x fail\r\n", task2_send_msg.msg_event);
			}
			else {
				pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Sending event 0x%04x, value %d to Task 2\r\n",
						task2_send_msg.msg_event, (int) task2_send_msg.msg_data );
			}
		}
	}
	else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply two integers > 0\r\n");
	}

	return pdFALSE;
}

#if( configGENERATE_RUN_TIME_STATS == 1 )

static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char * const pcHeader = "  Abs Time      % Time\r\n****************************************\r\n";
	BaseType_t xSpacePadding;

	/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, "Task" );
	pcWriteBuffer += strlen( pcWriteBuffer );

	/* Pad the string "task" with however many bytes necessary to make it the
		length of a task name.  Minus three for the null terminator and half the
		number of characters in	"Task" so the column lines up with the centre of
		the heading. */
	for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
	{
		/* Add a space to align columns after the task's name. */
		*pcWriteBuffer = ' ';
		pcWriteBuffer++;

		/* Ensure always terminated. */
		*pcWriteBuffer = 0x00;
	}

	strcpy( pcWriteBuffer, pcHeader );
	vTaskGetRunTimeStats( pcWriteBuffer + strlen( pcHeader ) );

	/* There is no more data to return after this single string, so return
		pdFALSE. */
	return pdFALSE;
}

#endif /* configGENERATE_RUN_TIME_STATS */

#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )

static BaseType_t prvQueryHeapCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	sprintf( pcWriteBuffer, "Current free heap %d bytes, minimum ever free heap %d bytes\r\n", ( int ) xPortGetFreeHeapSize(), ( int ) xPortGetMinimumEverFreeHeapSize() );

	/* There is no more data to return after this single string, so return
		pdFALSE. */
	return pdFALSE;
}

#endif /* configINCLUDE_QUERY_HEAP */


#if configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1

	static BaseType_t prvStartStopTraceCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
	{
	const char *pcParameter;
	BaseType_t lParameterStringLength;

		/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
		( void ) pcCommandString;
		( void ) xWriteBufferLen;
		configASSERT( pcWriteBuffer );

		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							1,						/* Return the first parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* There are only two valid parameter values. */
		if( strncmp( pcParameter, "start", strlen( "start" ) ) == 0 )
		{
			/* Start or restart the trace. */
			vTraceStop();
			vTraceClear();
			vTraceStart();

			sprintf( pcWriteBuffer, "Trace recording (re)started.\r\n" );
		}
		else if( strncmp( pcParameter, "stop", strlen( "stop" ) ) == 0 )
		{
			/* End the trace, if one is running. */
			vTraceStop();
			sprintf( pcWriteBuffer, "Stopping trace recording.\r\n" );
		}
		else
		{
			sprintf( pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n" );
		}

		/* There is no more data to return after this single string, so return
		pdFALSE. */
		return pdFALSE;
	}

#endif /* configINCLUDE_TRACE_RELATED_CLI_COMMANDS */


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

	xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(cli_task_id, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
	unsigned char rxChar;
	unsigned int index;     /* Index into cliBuffer */
	unsigned int x;
	char cliBuffer[CLI_CMD_LINE_BUF_SIZE];        /* Buffer for input */
	char output[CLI_OUTPUT_BUF_SIZE];        /* Buffer for output */
	BaseType_t xMore;

	DEV_UART_PTR dev_uart_ptr;
	DEV_BUFFER rx_buffer;

	memset(cliBuffer, 0, CLI_CMD_LINE_BUF_SIZE);
	memset(output, 0, CLI_OUTPUT_BUF_SIZE);
	index = 0;

	// TODO not a sensible place, but it gets us started
	exif_utc_clk_enable();

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

	for (;;) {

		// Reset the cliBuffer then re-enable the interrupts
		rx_buffer.buf = (void *) &rxChar;
		rx_buffer.len = NUMRXCHARACTERS;

		dev_uart_ptr->uart_control(UART_CMD_SET_RXINT_BUF, (UART_CTRL_PARAM) &rx_buffer);
		dev_uart_ptr->uart_control(UART_CMD_SET_RXINT, (UART_CTRL_PARAM) 1);

		// Hang here until ISR wakes us to process a character
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// process the character
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
			cliBuffer[index] = 0x00;

			// Evaluate the command - loop while the registered command returns true.
			// e.g. a "dir" command loops through for every directory entry
			do {
				memset(output, 0, CLI_OUTPUT_BUF_SIZE);
				xMore = FreeRTOS_CLIProcessCommand(cliBuffer, output, CLI_OUTPUT_BUF_SIZE);
				/* If xMore == pdTRUE, then output buffer contains no null termination, so
				 *  we know it is OUTPUT_BUF_SIZE. If pdFALSE, we can use strlen.
				 */
				//						for (x = 0; x < (xMore == pdTRUE ? OUTPUT_BUF_SIZE : strlen(output)) ; x++) {
				//							char outChar = *(output + x);
				//							if (outChar != 0) {
				//								putchar(outChar);
				//							}
				//						}
				// print the output[] cliBuffer until the first zero
				for (x = 0; x < CLI_OUTPUT_BUF_SIZE; x++) {
					char outChar = *(output + x);
					if (outChar == 0) {
						break;
					}
					putchar(outChar);
				}
			} while (xMore != pdFALSE);

			// New prompt
			index = 0;
			XP_YELLOW;
			xprintf("\ncmd> ");
			XP_WHITE;
			fflush(stdout);
			break;
		} // '\n\'

		default:
			// 'normal' characters
			if (index < CLI_CMD_LINE_BUF_SIZE) {
				putchar(rxChar);
				cliBuffer[index++] = rxChar;
				fflush(stdout);
			}
			else {
				// Throw away data and beep terminal
				putchar(0x07);
				fflush(stdout);
			}
			break;

		}	// switch
	}	// for(;;)
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
	FreeRTOS_CLIRegisterCommand( &xTask2 );

#if( configGENERATE_RUN_TIME_STATS == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xRunTimeStats );
	}
#endif

#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xQueryHeap );
	}
#endif

#if( configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xStartStopTrace );
	}
#endif
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
	if (xTaskCreate(vCmdLineTask, (const char *)"CLI",
			3 * configMINIMAL_STACK_SIZE + CLI_CMD_LINE_BUF_SIZE + CLI_OUTPUT_BUF_SIZE,
			NULL, tskIDLE_PRIORITY+1, &cli_task_id) != pdPASS)  {
		xprintf("xTaskCreate() failed to create a task.\n");
		configASSERT(0);	// TODO add debug messages?
	}
}

