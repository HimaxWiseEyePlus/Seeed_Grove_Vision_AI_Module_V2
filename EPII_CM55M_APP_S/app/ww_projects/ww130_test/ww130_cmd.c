/*
 * ww130_cmd.c
 *
 *  Created on: 30 Jul 2024
 *      Author: CGP
 *
 */

/*********************************** Includes ************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "xprintf.h"
#include "printf_x.h"
#include "ww130_cmd.h"
#include "comm_task.h"
#include "i2c_comm.h"
#include "evt_i2ccomm.h"
#include "hx_drv_iic.h"
#include "crc16_ccitt.h"
#include "main_task.h"

/*********************************** Definitions ************************************************/


/********************************* Structure define ************************************************/

// Define the structure for string/function pairs
struct ExpectedMsgStruct {
    char incomingString[WW130_MAX_PAYLOAD_SIZE];  // You can adjust the size according to your needs
    void (*processingFunction)(char * parameters);  // Pointer to a function with a string argumant and void return type
} expectedMsgStruct_t;

/*********************************** Local function declarations ************************************************/

static void command_callback(uint8_t * message);

static void processCmd(char *message);

// App Commands - Local Function Declarations
static void processEnable(char * parameters);
static void processDisable(char * parameters);
static void processStatus(char * parameters);
static void processSnap(char * parameters);
static void processExif(char * parameters);
static void processInt(char * parameters);

/*********************************** Local Variables ************************************************/

// Strings for the events - must align with aiProcessor_msg_type_t entries
const char *cmdString[] = {
		"None",
		"Received string",
		"Received base64",
		"Received binary",
		"Read string",
		"Read base64",
		"Read binary"	};

// This is an array, each element being a possible command from the I2C master, and the function to invoke
struct ExpectedMsgStruct expectedMessages[] = {
		{"status", processStatus},		// Send some status
		{"enable", processEnable},		// Enable reporting of sensor events
		{"disable", processDisable},	// Disable reporting of sensor events
		{"snap", processSnap},			// Take a picture
		{"exif", processExif},			// Send EXIF data to WW130
		{"int ", processInt}			// Ask board to pulse PA0 for a period (must be > 110ms)
};

uint8_t returnMessage[WW130_MAX_PAYLOAD_SIZE];
static bool sensorEnabled = false;

/********************************* App Commands - Local Function Definitions ********************/

static void processEnable(char * parameters) {
	sensorEnabled = true;
	snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Messages are enabled");
}

static void processDisable(char * parameters) {
	sensorEnabled = false;
	snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Messages are disabled");
}

static void processStatus(char * parameters) {
	snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Messages %s", sensorEnabled ? "enabled" : "disabled");
}

static void processSnap(char * parameters) {
	snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Taking a picture");
	main_startCapture();
}

static void processExif(char * parameters) {
	snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Sending EXIF data");
	main_sendExif();
}
/**
 * For testing the PA0 interrupt.
 * The phone/WW130 can ask this board to pulse the PA0 pin.
 *
 * It sends a parameter (duration in ms).
 * Experimentally I find that this period must be > 106ms
 * This seems very long and I wonder if it is related to switch debouncing code on the WW130.
 *
 */
static void processInt(char * parameters) {
	uint16_t duration;
	duration = atoi(parameters);

	if ((duration > 0) && (duration < 900)) {
		snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Pulsing PA0 for %dms ('%s')", duration, parameters);
		main_sendInt(duration);
	}
	else {
		snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Invalid parameter to pulse PA0 for %dms ('%s')", duration, parameters);
	}
}

/*********************************** Other Local Function Definitions ************************************************/

/**
 * Callback from comm_task when an I2C command arrives.
 * Called when the 1st byte is a certain value.
 *
 * This parses the command and acts upon it.
 *
 * @param - pointer to the incoming I2C message
 */
static void command_callback(uint8_t * message) {
	uint8_t feature;
	uint8_t cmd;
	uint16_t length;
	uint8_t * payload;
	bool crcOK;

	feature = message[I2CFMT_FEATURE_OFFSET];
	cmd = message[I2CFMT_COMMAND_OFFSET];
	length = (message[I2CFMT_PAYLOADLEN_LSB_OFFSET]) + ((message[I2CFMT_PAYLOADLEN_MSB_OFFSET] ) << 8);
	payload = &message[I2CFMT_PAYLOAD_OFFSET];

	crcOK = crc16_ccitt_validate(message, I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE );

	// Let's print the message
	printf_x_printBuffer(message, (I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE ));

	// Now overwrite the checksum with 0 so the payload becomes a string
	message[I2CCOMM_HEADER_SIZE + length] = '\0';

	if  ((feature != WW130_CMD_FEATURE) || (cmd >= AI_PROCESSOR_MSG_END)) {
		xprintf("Received feature=%d cmd=%d length=%d \n", feature, cmd, length);
		xprintf("Command is not recognised\n");
		return;
	}

	xprintf("Received feature=%d cmd=%d (%s) length=%d '%s' ",
			feature, cmd, cmdString[cmd], length, (char * ) payload);

	// Check the CRC
	// TODO move CRC checking to comm_task?
	if(crcOK) {
		xprintf("CRC OK\n");
	}
	else {
		xprintf("CRC failed\n");
		return;
	}

	// Default message, which hopefully will be over-written
	snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Unrecognised command");

	// Now let's see if we can make sense of the message
	switch (cmd) {
	case AI_PROCESSOR_MSG_TX_STRING:
		// I2C master has sent a string. We assume it is a command to execute
		xprintf("WW130 command received: '%s'\n", (char * ) payload);
		processCmd((char *) payload);
		break;

	case AI_PROCESSOR_MSG_RX_STRING:
		// I2C master has requested a string.
		//i2cs_slave_if_send_string((char *) returnMessage);	// prepare to send buffer to the master
		break;

	// Not yet implemented - do nothing
	case AI_PROCESSOR_MSG_TX_BINARY:
	case AI_PROCESSOR_MSG_RX_BINARY:
	default:
		// nothing defined yet
		break;
	}

	xprintf("Response string: '%s'\n", (char * ) returnMessage);
	comm_task_customer_response(returnMessage, AI_PROCESSOR_MSG_RX_STRING,
			strnlen((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE - 1) + 1);	// return the string, including the trailing '\0'
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
	memset(returnMessage, 0, WW130_MAX_PAYLOAD_SIZE);

	for (uint8_t i=0; i < numMessages; i++) {
		thisEntry = expectedMessages[i];
		expectedLength = strlen(thisEntry.incomingString);

		comparison = strncmp( message, thisEntry.incomingString, expectedLength );
		if (comparison == 0) {
			// Extra check: either the last character is a space, or the strlen is exact (e.g. rejects "idd")

			if ((message[expectedLength - 1] == ' ')) {
				// probably further characters to be parsed, so pass a pointer to them as a parameter
				thisEntry.processingFunction(&message[expectedLength]);
				break;
			}

			if (strlen(message) == expectedLength) {
				// No further characters in the string
				thisEntry.processingFunction(NULL);
				break;
			}
		}
	}

	if (strlen((char *) returnMessage) == 0) {
		// Unrecognised command
		xprintf("Unrecognised command '%s'\n", message);
		snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "?");
	}
	// The response message is now in returnMessage[]
	// We now wait for the I2C read from the WW130 to return the message
}


/*********************************** Exported Function Definitions ************************************************/

// Called once by comm_task
void ww130_cmd_init(void) {
	comm_task_customer_register_cb(command_callback);
}

