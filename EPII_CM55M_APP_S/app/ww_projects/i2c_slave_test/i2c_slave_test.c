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
 */

/********************************* Includes ******************************/
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "xprintf.h"
#include "print_x.h"	// supports colour

#include "i2c_slave_if.h"
#include "i2c_slave_commands.h"
#include "WE2_core.h"

/********************************* Local Defines **************************************************/

// Size of buffer for sending and receiving binary data
#define BINARY_BUFF_SIZE	1000

/******************************** Local Function Declarations **************************************/

static void i2cEventCallback(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length);
static void fillBuffer(void);

/********************************* Local Variables ***********************************************/

// This is a buffer for testing, and is local to this file.
// Pointers to this buffer are passed to i2c_slave_if.c
static uint8_t binaryBuffer[BINARY_BUFF_SIZE];

/********************************* External Variables ***********************************************/

extern const char *i2c_slave_if_msgTypeStr[];

/********************************* Local Function Definitions ***************************/


/**
 * Callback from i2c_slave_if when an event occurs
 *
 * Also called when internal events occur (e.g. all binary data received from the BLE app)
 *
 */
static void i2cEventCallback(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length) {

	if (i2cs_slave_if_inISR()) {
		xprintf("In ISR (%s)\n", __FUNCTION__);
	}
	else {
		xprintf("NOT In ISR (%s)\n", __FUNCTION__);
	}

	switch (cmd) {
	// All processed elsewhere
	case AI_PROCESSOR_MSG_NONE:
	case AI_PROCESSOR_MSG_TX_STRING:
	case AI_PROCESSOR_MSG_TX_BASE64:
	case AI_PROCESSOR_MSG_TX_BINARY:
	case AI_PROCESSOR_MSG_RX_STRING:
	case AI_PROCESSOR_MSG_RX_BASE64:
	case AI_PROCESSOR_MSG_RX_BINARY:
	case AI_PROCESSOR_MSG_END:
		xprintf("Noted '%s' message\n", i2c_slave_if_msgTypeStr[cmd]);
		break;

	case AI_PROCESSOR_MSG_TX_BINARY_COMPLETE:
		// The BLE app has transferred all expected binary data to our buffer
		// In a real application the app can now act upon this binary data.
		xprintf("Received '%s' event. All binary data has now arrived (%d bytes)\n\n", i2c_slave_if_msgTypeStr[cmd], length);

		// to show this has worked, print the data
		i2cs_slave_if_printBuffer(binaryBuffer, length);
		break;

	case AI_PROCESSOR_MSG_RX_BINARY_COMPLETE:
		// The BLE app has transferred all expected binary data from our buffer
		// In a real application the app can now free the buffer etc.
		xprintf("Received '%s' event. All binary data has been sent (%d bytes)\n", i2c_slave_if_msgTypeStr[cmd], length);
		break;

	case AI_PROCESSOR_MSG_PA0:
		// PA0 pin set low by WW130
		P_RED;
		xprintf("Received '%s' event\n", i2c_slave_if_msgTypeStr[cmd]);
		P_WHITE;
		break;

	default:
		// Not expected
		break;
	}
}

/**
 * Fill the buffer with some test text.
 */
static void fillBuffer(void) {
	char * sonnet29 = "When, in disgrace with fortune and men's eyes, "
			"I all alone beweep my outcast state "
			"And trouble deaf heaven with my bootless cries "
			"And look upon myself and curse my fate,"
			"Wishing me like to one more rich in hope, "
			"Featured like him, like him with friends possessed, "
			"Desiring this man’s art and that man’s scope,"
			"With what I most enjoy contented least;  "
			"Yet in these thoughts myself almost despising,"
			"Haply I think on thee, and then my state,"
			"(Like to the lark at break of day arising"
			"From sullen earth) sings hymns at heaven’s gate;"
			"For thy sweet love remembered such wealth brings"
			"That then I scorn to change my state with kings.";
	strncpy((char *) binaryBuffer, sonnet29, BINARY_BUFF_SIZE);
}

/********************************* app_main() ******************************************/

/*!
 * @brief Main function
 *
 * This code initialises the I2C slave and waits for I2C slave messages.
 */
int app_main(void) {

	P_YELLOW;
	xprintf("\n\n *** I2C Slave Interface Test ***   Built: %s %s ***\n", __DATE__, __TIME__);
	P_WHITE;

	// This tests that we can determine whether or not we are inside an interrupt service routine...
	if (i2cs_slave_if_inISR()) {
		xprintf("In ISR (%s)\n", __FUNCTION__);
	}
	else {
		xprintf("NOT In ISR (%s)\n", __FUNCTION__);
	}

	fillBuffer();	// put some text in the buffer, so we can read it back for test purposes

	// Pass callback and buffer details to lower levels
	i2c_slave_commands_init(i2cEventCallback, binaryBuffer, BINARY_BUFF_SIZE);

	while(1);

	return 0;
}

