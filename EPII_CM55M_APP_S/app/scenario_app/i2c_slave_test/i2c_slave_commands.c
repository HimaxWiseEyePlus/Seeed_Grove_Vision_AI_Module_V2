/*
 * i2c_slave_commands.c
 *
 *  Created on: 21/07/2024
 *      Author: CGP
 *
 * This app provides a way to test the interface between the WW130 and this board.
 *
 * Most of the work is in 12c_slave_if.c which can be used in real applications.
 *
 *
 * Intermediate processing between client and i2c_slave_if.c
 *
 */


/********************************* Includes ****************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "xprintf.h"
#include "print_x.h"

#include "i2c_slave_if.h"
#include "i2c_slave_commands.h"

/********************************* Local Defines **************************************************/


/******************************** Local Function Declarations **************************************/

bool commandsCallback(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length);

// App Commands - Local Function Declarations
// The 'procesXXX' calls are requests from the BLE app to perform functions

static void processStatus(char * parameter);
static void processEnable(char * parameter);
static void processDisable(char * parameter);
static void processTestPA0(char * parameter);
static void processGetExif(char * parameter);

/********************************* Local Variables ***********************************************/

// Define and initialize the array of ExpectedMsgStruct with static elements.
// These are strings to search for plus functions to execute if a match is found.
static struct ExpectedMsgStruct expectedMessages[] = {
		{"status", processStatus},		// Send some status
		{"enable", processEnable},		// Enable reporting of sensor events
		{"disable", processDisable},	// Disable reporting of sensor events
		{"test pa0 ", processTestPA0},	// For testing only: asserts the PA0 pin to the WW130
		{"get exif", processGetExif},	// Get EXIF data
};

static bool sensorEnabled = false;

/********************************* External Variables ***********************************************/

extern char i2c_slave_if_returnMessage[I2C_SLAVE_IF_PAYLOAD_SIZE];
extern const char *i2c_slave_if_msgTypeStr[];

/********************************* App Commands - Local Function Definitions ********************/

static void processStatus(char * parameter) {
	xprintf("App sent 'status'\n");	// debug only
	snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Sensor messages %s", sensorEnabled ? "enabled" : "disabled");
}

static void processEnable(char * parameter) {
	xprintf("App sent 'enable'\n");	// debug only
	sensorEnabled = true;
	snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Sensor messages are enabled");
}

static void processDisable(char * parameter) {
	xprintf("App sent 'disable'\n");	// debug only
	sensorEnabled = false;
	snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Sensor messages are disabled");
}

static void processTestPA0(char * parameter) {
	uint16_t pulseWidth;

	pulseWidth = atoi(parameter);

	if (pulseWidth == 0) {
		xprintf("App sent 'test pa0 ' - duration missing\n");
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: no parameter");
	}
	else {
		xprintf("App sent 'test pa0 ' - for %dms\n", pulseWidth);	// debug only
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Pulsing PA0 for %dms", pulseWidth);
		i2cs_slave_wake_master(pulseWidth);
	}
}

static void processGetExif(char * parameter) {
	xprintf("App sent 'get exif'\n");	// debug only
	snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "TBD");
}

/********************************* Other Local Function Definitions ***************************/

/**
 * Callback from i2c_slave_if when an I2C command arrives.
 *
 * Also called when internal events occur (e.g. all binary data received from the BLE app)
 *
 * This attempts to parse the command and act upon it.
 * @return true if the command was processed here.
 */
bool commandsCallback(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length) {
	uint16_t numMessages;

	if (cmd >=AI_PROCESSOR_MSG_END) {
		// illegal
		return false;
	}

	// Add a little colour for clarity
	P_GREEN;
	xprintf("Received '%s' message ", i2c_slave_if_msgTypeStr[cmd]);
	P_WHITE;
	xprintf("(length=%d)\n", length);

	switch (cmd) {
	case AI_PROCESSOR_MSG_TX_STRING:
		// I2C master has sent a string. We assume it is a command to execute
		numMessages = sizeof(expectedMessages)/sizeof(*expectedMessages);
		//xprintf("DEBUG: in i2c_slave_test.c there are %d possible messages\n", numMessages);
		return i2cs_slave_if_processCommandString((char *) message, expectedMessages, numMessages);

	case AI_PROCESSOR_MSG_RX_STRING:
		// I2C master has requested a string. If we have one to send then send it
		if (strlen(i2c_slave_if_returnMessage) > 0 ) {
			i2cs_slave_if_send_string(i2c_slave_if_returnMessage);	// prepare to send buffer to the master
			return true;
		}
		else {
			return false;
		}

	case AI_PROCESSOR_MSG_TX_BINARY_COMPLETE:
		// The BLE app has transferred all expected binary data to us
		// In a real application the app can now act upon this binary data.
		xprintf("All binary data has now arrived: %d bytes\n", length);
		return true;

	case AI_PROCESSOR_MSG_RX_BINARY_COMPLETE:
		// The BLE app has received all expected binary data to us
		xprintf("Binary data has now been sent: %d bytes\n", length);
		return true;

	// Not implemented in this file - return false
	case AI_PROCESSOR_MSG_TX_BASE64:
	case AI_PROCESSOR_MSG_TX_BINARY:
	case AI_PROCESSOR_MSG_RX_BINARY:
	case AI_PROCESSOR_MSG_NONE:
	default:
		// nothing defined yet
		return false;
	}
}


/****************************** Exported Function Definitions ************************/

// Called by client to register callback and initialise i2c_slave_if.c
void i2c_slave_commands_init(void * clientCallback, uint8_t * clientBuffer, uint16_t clientBufferSize) {

	// Pass on the callback:
	i2c_slave_if_init(clientCallback, commandsCallback);

	// Initialise buffer pointers etc with the test buffer
	i2cs_slave_if_binaryBufferInit(clientBuffer, clientBufferSize);
}

