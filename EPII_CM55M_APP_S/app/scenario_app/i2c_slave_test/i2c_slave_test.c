/*
 * i2c_slave_test.c
 *
 *  Created on: 11/07/2024
 *      Author: charl
 *
 * This app provides a way to test the interface between the WW130 and this board.
 *
 * Most of the work is in 12c_slave_if.c which can be used in real applications.
 *
 *
 */


/********************************* Includes ******************************/
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#include "xprintf.h"

#include "i2c_slave_if.h"

/********************************* Local Defines **************************************************/


/********************************* Structure define ************************************************/

// Define the structure
struct ExpectedMsgStruct {
    char incomingString[I2C_SLAVE_IF_BUFF_SIZE];  // You can adjust the size according to your needs
    void (*processingFunction)(void);  // Pointer to a function with no arguments and void return type
} expectedMsgStruct_t;

/******************************** Local Function Declarations **************************************/

void command_callback(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length);
static void processCmd(char *message);

// App Commands - Local Function Declarations
static void processEnable(void);
static void processDisable(void);
static void processStatus(void);

/********************************* Local Variables ***********************************************/

// Strings for the events - must align with aiProcessor_msg_type_t entries
const char *cmdString[] = {
		"Received string",
		"Received binary",
		"Read string",
		"Read binary"	};

// Define and initialize the array of MyStruct with static elements

struct ExpectedMsgStruct expectedMessages[] = {
		{"status", processStatus},		// Send some status
		{"enable", processEnable},		// Enable reporting of sensor events
		{"disable", processDisable}	// Disable reporting of sensor events
};

uint8_t returnMessage[I2C_SLAVE_IF_BUFF_SIZE];
static bool sensorEnabled = false;

/********************************* App Commands - Local Function Definitions ********************/

static void processEnable(void) {
	xprintf("App sent 'enable'\n");	// debug only
	sensorEnabled = true;
	snprintf((char *) returnMessage, I2C_SLAVE_IF_BUFF_SIZE, "Sensor messages are enabled");
}

static void processDisable(void) {
	xprintf("App sent 'disable'\n");	// debug only
	sensorEnabled = true;
	snprintf((char *) returnMessage, I2C_SLAVE_IF_BUFF_SIZE, "Sensor messages are disabled");
}

static void processStatus(void) {
	xprintf("App sent 'status'\n");	// debug only
	snprintf((char *) returnMessage, I2C_SLAVE_IF_BUFF_SIZE, "Sensor messages %s", sensorEnabled ? "enabled" : "disabled");
}

/********************************* Other Local Function Definitions ***************************/


/**
 * Callback from i2c_slave_if when an I2C command arrives.
 *
 * This parses the command and acts upon it.
 *
 */
void command_callback(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length) {

	if (cmd >=AI_PROCESSOR_MSG_END) {
		// illegal
		return;
	}

	xprintf("Received '%s' message (length %d)\n", cmdString[cmd], length);

	switch (cmd) {
	case AI_PROCESSOR_MSG_TX_STRING:
		// I2C master has sent a string. We assume it is a command to execute
		//xprintf("String received: '%s'\n", (char * ) message);
		processCmd((char *) message);
		break;


	case AI_PROCESSOR_MSG_RX_STRING:
		// I2C master has requested a string.
		i2cs_slave_if_send_string((char *) returnMessage);	// prepare to send buffer to the master
		break;

	// Not yet implemented - do nothing
	case AI_PROCESSOR_MSG_TX_BINARY:
	case AI_PROCESSOR_MSG_RX_BINARY:
	default:
		// nothing defined yet
		break;
	}
}

/**
 * The app has sent us a command an expects a response.
 *
 * This routine steps through a table to find the matching command,
 * then calls the designated function.
 *
 * Whatever happens, a response string is prepared and will be sent later,
 * when the I2C master requests a string to be returned.
 *
 * @param message - message sent by the app
 */
static void processCmd(char *message) {

	struct ExpectedMsgStruct thisEntry;
	int16_t expectedLength;
	uint8_t numMessages;
	uint8_t comparison;

	// Thi sis the number of possible strings that will be checked,
	// i.e. it is the length of expectedMessages[]
	numMessages = sizeof(expectedMessages)/sizeof(expectedMsgStruct_t);

	// empty the string
	memset(returnMessage, 0, I2C_SLAVE_IF_BUFF_SIZE);

	for (uint8_t i=0; i < numMessages; i++) {
		thisEntry = expectedMessages[i];
		expectedLength = strlen(thisEntry.incomingString);

		comparison = strncmp( message, thisEntry.incomingString, expectedLength );
		if (comparison == 0) {
			// Extra check: either the last character is a space, or the strlen is exact (e.g. rejects "idd")

			if ((message[expectedLength - 1] == ' ') || (strlen(message) == expectedLength)) {
				thisEntry.processingFunction();
				break;
			}
		}
	}

	if (strlen((char *) returnMessage) == 0) {
		// Unrecognised command
		xprintf("Unrecognised command '%s'\n", message);
		snprintf((char *) returnMessage, I2C_SLAVE_IF_BUFF_SIZE, "?");
	}
	// The response message is now in returnMessage[]
	// We now wait for the I2C read from the WW130 to return the message
}

/********************************* app_main() ********************/

/*!
 * @brief Main function
 *
 * This code initialises the I2C slave and waits for I2C slave messages.
 */
int app_main(void) {

	xprintf("\n\n *** Start I2C Slave Test. ***   Built: %s %s ***\n", __DATE__, __TIME__);
	i2c_slave_if_init(command_callback);

	while(1);

	return 0;
}

