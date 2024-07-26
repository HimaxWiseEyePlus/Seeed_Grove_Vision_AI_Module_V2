/*
 * File: i2c_slave_if.c
 *
 *  Created on: 11/07/2024
 *      Author: charl
 *
 * This contains functions that support inter-board communication
 *
 * Note that if we decide to use FreeRTOS, then the project 'allon_sensor_fflm_freertos'
 * includes a class that supports I2C slave - see comm_task.c
 *
 */

/********************************* Includes ******************************/

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// for isprint():
#include "ctype.h"

#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
#endif
#endif

#include "xprintf.h"
#include "print_x.h"
#include "WE2_core.h"
#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "hx_drv_scu.h"
#include "hx_drv_timer.h"
#include "hx_drv_iic.h"
#include "i2c_slave_if.h"

#include "hx_drv_gpio.h"

#include "base64.h"


/********************************* Local Defines ******************************/
// TODO is this necessary?
#define I2CS_TX_TIME_OUT_MS				1000

// Time in us
// TODO work out what this should be
#define PULSELOWTIME	100	//

// Slave ID address. Must be the same as the WW130 uses
#define I2CS_0_ID	0x62

// Number of data bytes printed in a single line.
#define IN_LINE_PRINT_CNT   16

/********************************* Local Function Declarations ********************/

static void initI2CSlave(void);
static void i2cs_cb_tx(void *param);
static void i2cs_cb_rx(void *param);
static void i2cs_cb_err(void *param);
static void i2cs_tx_timeout_cb(void *data);

static void i2cs_write_enable(uint32_t size);
static void i2cs_read_enable(uint32_t size);
static void i2cs_tx_timer(uint32_t timeout_ms);

static void aon_gpio0_cb(uint8_t group, uint8_t aIndex);

static bool processCmd(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length);
//static bool i2cSlave_processCommandString(char *message);
static void processBase64(char *message);
static void processBinary(uint8_t * message, uint16_t numBytes );
static void processTxBinary(char * parameter);
static void processRxBinary(char * parameter);
static void processRequestBinary(char * parameter);
static void processRequestBase64(char * parameter);

static void sendBinaryData(void);

static void pa0TimeoutCb(void *data);

// Callback function prototype
typedef bool (*CallbackFunction)(uint8_t cmd, uint8_t *message, uint16_t length);

/********************************* Local Variables *********************************/

// Strings for the events - must align with aiProcessor_msg_type_t entries
const char *i2c_slave_if_msgTypeStr[] = {
		"None",
		"Received string",
		"Received base64",
		"Received binary",
		"Read string",
		"Read base64",
		"Read binary",
		"Binary TX complete",
		"Binary RX complete",
		"PA0 event",
		"End of list"
};

static uint32_t i2c_s_id;
static HX_DRV_DEV_IIC *dev_iic_slv;
static uint32_t slave_addr;
static uint8_t rx_buffer[I2C_SLAVE_IF_BUF_SIZE];	// allows for 3 ctrl bytes and a payload
static uint8_t tx_buffer[I2C_SLAVE_IF_BUF_SIZE];

// buffer for text messages to return to the BLE app
char i2c_slave_if_returnMessage[I2C_SLAVE_IF_PAYLOAD_SIZE];

static CallbackFunction clientCallback = NULL;
static CallbackFunction commandsCallback = NULL;

// Pointers for the binary buffer
uint8_t * m_binaryBuf;		// The pointer to the binary buffer (used for sending and receiving)
uint16_t m_binaryBufIndex;	// The current position in the binary buffer for the next transfer
uint16_t m_binaryBufSize;	// The size of the buffer of binary data
uint16_t m_expectedBytes;	// The total number of bytes to send or receive (excluding the 3 command bytes)
uint16_t m_requestedBytes;	// The number of bytes to send in the next request

// Define and initialize the array of ExpectedMsgStruct with static elements.
// These are strings to search for plus functions to execute if a match is found.
static struct ExpectedMsgStruct expectedMessages[] = {
		{"tx binary ", 	processTxBinary}, // BLE app to begin tranmsission of binary data
		{"rx binary ", 	processRxBinary},  // BLE app to begin reception of binary data
		{"rx * ", 		processRequestBinary},  // BLE app request binary data
		{"rx ! ", 		processRequestBase64},  // BLE app request base64 data
};

/********************************* Local Function Definitions ********************/


/**
 * Initialise the I2C slave module
 */
static void initI2CSlave(void) {
	// This is the slave instance we will use
    i2c_s_id = USE_DW_IIC_SLV_0;
	slave_addr = I2CS_0_ID;
    hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_I2C_S_SCL_0, 1);
    hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_I2C_S_SDA_0, 1);

    xprintf("I2C Slave 0 Init: Slave address = 0x%02x\n", slave_addr);

    hx_drv_i2cs_init(i2c_s_id, HX_I2C_HOST_SLV_0_BASE);

    dev_iic_slv = hx_drv_i2cs_get_dev(i2c_s_id);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_TXCB, (void *) i2cs_cb_tx);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_RXCB, (void *) i2cs_cb_rx);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_ERRCB, (void *) i2cs_cb_err);
	dev_iic_slv->iic_control(DW_IIC_CMD_SLV_SET_SLV_ADDR, (void*)slave_addr);

	// Prepare for incoming I2C message. Looks like we need to do this only once.
	i2cs_read_enable(I2C_SLAVE_IF_BUF_SIZE);
}

/**
 * Callback from timer if I2C master does not read data we have prepared for it.
 *
 *
 */
static void i2cs_tx_timeout_cb(void *data) {
	// if i2c slave tx complete timeout, reset i2c slave
    xprintf("\n\n#### [%s], i2cs_reset ####\n\n", __FUNCTION__);

    // Is this the right thing to do?
    hx_drv_i2cs_reset(i2c_s_id);
    // Reinitialise:
    initI2CSlave();
}

/**
 * Prepares a timer that can expire if the master does not read
 * the slave's data within a time period.
 *
 * The timer callback is defined here.
 */
static void i2cs_tx_timer(uint32_t timeout_ms) {
	TIMER_CFG_T timer_cfg;
	timer_cfg.period = timeout_ms;
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_CPU;
	timer_cfg.state = TIMER_STATE_DC;

    hx_drv_timer_cm55s_stop();
	hx_drv_timer_cm55s_start(&timer_cfg, (Timer_ISREvent_t)i2cs_tx_timeout_cb);
}

/**
 * Callback that occurs once the master has read data from the slave.
 *
 * This executes in ISR context.
 */
static void  i2cs_cb_tx(void* param) {
	HX_DRV_DEV_IIC*      iic_obj;
	HX_DRV_DEV_IIC_INFO* iic_info_ptr;

	uint16_t outstandingBytes;
	uint16_t numBytes;
	uint16_t offset;

    iic_obj      = (HX_DRV_DEV_IIC*) param;
    iic_info_ptr = &(iic_obj->iic_info);
    numBytes = (uint16_t) iic_info_ptr->tx_buf.len;
    offset = (uint16_t) iic_info_ptr->tx_buf.ofs;

	xprintf("\n[%s] Sent %d bytes from a buffer of %d\n", __FUNCTION__,
			(uint16_t) offset, (uint16_t) numBytes);

	hx_drv_timer_cm55s_stop();

	if (tx_buffer[0] == AI_PROCESSOR_MSG_RX_BINARY) {
		// We have completed a binary transfer. Update pointers etc (for multi-part transfers)
		// I expect that numBytes is 3 more than the payload bytes, so let's make that adjustment:
		if (numBytes >= I2C_SLAVE_IF_CTRL_BYTES) {
			numBytes -= I2C_SLAVE_IF_CTRL_BYTES;
		}
		m_binaryBufIndex += numBytes;
		outstandingBytes = m_expectedBytes - m_binaryBufIndex;

		if (outstandingBytes == 0) {
			// Finished
			xprintf("Sent %d bytes. Complete.\n", numBytes);
			snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE,
					"Sent %d bytes. Complete.", numBytes);

			// Perform a callback so i2c_slave_commands.c knows the data is complete
			if (commandsCallback != NULL) {
				commandsCallback(AI_PROCESSOR_MSG_RX_BINARY_COMPLETE, NULL, m_expectedBytes);
			}

			// Perform a callback so the client knows the data is complete
			if (clientCallback != NULL) {
				clientCallback(AI_PROCESSOR_MSG_RX_BINARY_COMPLETE, NULL, m_expectedBytes);
			}
		}
		else {
			xprintf("Sent %d bytes. %d expected %d outstanding\n", numBytes, m_expectedBytes, outstandingBytes);
			snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Sent %d bytes. %d expected %d left",
					numBytes, m_expectedBytes, outstandingBytes);
		}
		tx_buffer[0] = AI_PROCESSOR_MSG_NONE;	// Just to be sure we don't execute this again
	}
}

/**
 * This function is called whenever the I2C slave is written to by the master.
 * Received data is available in rx_buffer[]
 *
 * I have made the size of rx_buffer larger by the 3 expected parameter bytes
 *
 * This executes in ISR context.
 *
 */
static void  i2cs_cb_rx(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    uint8_t cmd;
    uint8_t payloadLen;
    uint32_t length;
    uint32_t offset;
    bool processed = false;

    length = iic_info_ptr->rx_buf.len;
    offset = iic_info_ptr->rx_buf.ofs;	// This seems to be the actual size of the message

	xprintf("\n[%s] Received %d bytes. (RX buffer size: %d)\n", __FUNCTION__,
			(uint16_t) offset, (uint16_t) length);

	i2cs_slave_if_printBuffer(rx_buffer, offset);

	if (length < I2C_SLAVE_IF_CTRL_BYTES) {
		// malformed message
		xprintf("Mal-formed message - too short\n");
		return;
	}

	// Parameter bytes arrive first
    cmd = rx_buffer[0];
    payloadLen = rx_buffer[1] + (rx_buffer[2] << 8);

    // Not sure if this s required, but to be safe I am adding '\0' to ensure this is a string
    if ((I2C_SLAVE_IF_CTRL_BYTES + payloadLen) < I2C_SLAVE_IF_BUF_SIZE) {
    	rx_buffer[I2C_SLAVE_IF_CTRL_BYTES + payloadLen] = '\0';
    }

    xprintf("   Parameters: cmd: %d, len: %d\n", cmd, payloadLen );

    // Give i2c_slave_commands.c the opportunity to process this message.
    if (commandsCallback != NULL) {
    	processed = commandsCallback(cmd, &rx_buffer[I2C_SLAVE_IF_CTRL_BYTES], offset - I2C_SLAVE_IF_CTRL_BYTES);
    }

    if (!processed) {
    	// try to process it in this file
    	processed = processCmd(cmd, &rx_buffer[I2C_SLAVE_IF_CTRL_BYTES], offset - I2C_SLAVE_IF_CTRL_BYTES);
    }

    if (!processed) {
    	snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: unrecognised command");
    }

    // Let the client know as well
    if (clientCallback != NULL) {
    	clientCallback(cmd, &rx_buffer[I2C_SLAVE_IF_CTRL_BYTES], offset - I2C_SLAVE_IF_CTRL_BYTES);
    }

    // Now clear the rx_buffer
	memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * This is the I2C error callback.
 */
static void  i2cs_cb_err(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    xprintf("[%s], err_code = %d\n", __FUNCTION__, iic_info_ptr->err_state);
    P_RED;
    xprintf("TODO - fix this up so it works for the WW app\n");
    P_WHITE;

	// i2c master read but there is no "CMD" from i2c master,
    // return previous result to avoid clock stretching
	if ( iic_info_ptr->err_state == DEV_IIC_ERR_TX_DATA_UNREADY) {
		xprintf("\nDEV_IIC_ERR_TX_DATA_UNREADY\n");
        i2cs_write_enable(8);
    }
}

/**
 * Initialise I2C slave reading.
 *
 * It looks like this only needs to be called once.
 * size defines the maximum number of bytes to be read i.e. the max number of bytes the I2C master should write.
 */
static void i2cs_read_enable(uint32_t size) {
	memset(rx_buffer, 0, sizeof(rx_buffer));
    hx_drv_i2cs_interrupt_read(i2c_s_id, slave_addr, rx_buffer, size, (void*) i2cs_cb_rx);
}

/**
 * Prepare the I2C slave to send data to the master.
 *
 * A timer is started. If the master does not read the data before the timer expires,
 * a timer error is triggered.
 *
 * Prepares a buffer of suitable size for the expected outgoing data.
 */
static void i2cs_write_enable(uint32_t size) {
	hx_drv_i2cs_interrupt_write(i2c_s_id, slave_addr, tx_buffer, size, (void*) i2cs_cb_tx);
	//register a tx timeout callback to avoid i2c slave halted
	i2cs_tx_timer(I2CS_TX_TIME_OUT_MS);
}

/**
 * Interrupt service routine for PA0
 *
 * This is called when the WW130 wakes this board by toggling PA0
 */
static void aon_gpio0_cb(uint8_t group, uint8_t aIndex) {
    uint8_t gpio_value;

    hx_drv_gpio_get_in_value(AON_GPIO0, &gpio_value);
    xprintf("PA0 Interrupt Pin is %d. (Group: %d, aIndex: %d)\n", gpio_value, group, aIndex);

	hx_drv_gpio_clr_int_status(AON_GPIO0);

	// Perform a callback so the client knows the data is complete
	if (clientCallback != NULL) {
		clientCallback(AI_PROCESSOR_MSG_PA0, NULL, 0);
	}
}

/**
 * Processes a command from the I2C slave
 *
 * There is an equivalent function in
 *
 * @param cmd = the first command byte in the I2C message
 * @param message = the following message (after the command bytes)
 * @param length = the length of the message (after the command bytes)
 * @return true if the command is processed in this function
 */
static bool processCmd(aiProcessor_msg_type_t cmd, uint8_t * message, uint16_t length) {
	uint16_t numMessages;
	bool processed;

	if (cmd >=AI_PROCESSOR_MSG_END) {
		// illegal
		return false;
	}

	xprintf("Received '%s' message (length=%d)\n", i2c_slave_if_msgTypeStr[cmd], length);

	switch (cmd) {
	case AI_PROCESSOR_MSG_TX_STRING:
		// I2C master has sent a string. We assume it is a command to execute
		//xprintf("String received: '%s'\n", (char * ) message);
		numMessages = sizeof(expectedMessages)/sizeof(*expectedMessages);
		//xprintf("DEBUG: in i2c_slave_if.c there are %d possible messages\n", numMessages);
		processed = i2cs_slave_if_processCommandString((char *) message, expectedMessages, numMessages);

		if (!processed) {
			// Need to add an error message
			snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: command '%s' not recognised", message);
		}
		return processed;


	case AI_PROCESSOR_MSG_RX_STRING:
		// I2C master has requested a string. If we have one to send then send it
		if (strlen(i2c_slave_if_returnMessage) > 0 ) {
			i2cs_slave_if_send_string(i2c_slave_if_returnMessage);	// prepare to send buffer to the master
			return true;
		}
		else {
			return false;
		}

	case AI_PROCESSOR_MSG_TX_BASE64:
		// I2C master has sent binary data encoded as Base64
		xprintf("Base64 data received: '%s'\n", (char * ) message);
		processBase64((char *) message);
		// hack return true for now
		return true;

	case AI_PROCESSOR_MSG_TX_BINARY:
		// I2C master has sent binary data
		xprintf("Binary data received (%d bytes)\n", length);
		processBinary(message, length);
		// hack return true for now
		return true;


	case AI_PROCESSOR_MSG_RX_BINARY:
		// I2C master requests data from the binary buffer
		sendBinaryData();
		return true;

	// Not yet implemented - do nothing
	case AI_PROCESSOR_MSG_NONE:
	default:
		// nothing defined yet
		return false;
		break;
	}
}

/**
 * The app has sent us a command and probably expects a response.
 *
 * This routine steps through a table to find the matching command,
 * then calls the designated function.
 *
 * If a match is not fund then this returns false, allowing a different table of commands to be searched
 *
 * Whatever happens, a response string is prepared and will be sent later,
 * when the I2C master requests a string to be returned.
 *
 * @param message - message sent by the app
 * @return true if a command is recognised and processed
 */
bool i2cs_slave_if_processCommandString(char *message, struct ExpectedMsgStruct possibleMessages[], uint16_t numMessages) {
	struct ExpectedMsgStruct thisEntry;
	int16_t expectedLength;
	//uint8_t numMessages;
	uint8_t comparison;

	// Clear the return message
	memset(i2c_slave_if_returnMessage, 0, I2C_SLAVE_IF_PAYLOAD_SIZE);

	for (uint8_t i=0; i < numMessages; i++) {
		thisEntry = possibleMessages[i];
		expectedLength = strlen(thisEntry.incomingString);

		comparison = strncmp( message, thisEntry.incomingString, expectedLength );
		if (comparison == 0) {
			// Extra check: either the last character is a space, or the strlen is exact (e.g. rejects "idd")

			if ((message[expectedLength - 1] == ' ') || (strlen(message) == expectedLength)) {
				thisEntry.processingFunction(&message[expectedLength]);
				return true;
			}
		}
	}

	return false;
}

/**
 * The app has sent us binary data as a base64 string, and probably expects a response.
 *
 * Various checks are made to ensure the data can be appended to the final destination buffer.
 * If OK the base64 message is decoded and added to that buffer, and pointers etc updated.
 *
 * Whatever happens, a response string is prepared and will be sent later.
 * when the I2C master requests a string to be returned.
 *
 * @param message - binary data as base64 sent by the app
 */
static void processBase64(char * message) {
	// This is a temporary buffer to accept the decoded binary data
	uint8_t decodedBinary[I2C_SLAVE_IF_PAYLOAD_SIZE];
	size_t olen;
	uint16_t length;
	int16_t outstandingBytes;
	int ret;

	// Clear the return message
	memset(i2c_slave_if_returnMessage, 0, I2C_SLAVE_IF_PAYLOAD_SIZE);

	length = strnlen(message, I2C_SLAVE_IF_PAYLOAD_SIZE);
	if (length == I2C_SLAVE_IF_PAYLOAD_SIZE) {
		// Not a string - no room for '\0'
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: parameter too long");
	}
	else {
		ret = mbedtls_base64_decode((unsigned char *) decodedBinary, I2C_SLAVE_IF_PAYLOAD_SIZE, &olen,
				(const unsigned char *) message, length);
		if (ret == 0) {
			// Now try to add it to the binary buffer
			outstandingBytes = m_expectedBytes - m_binaryBufIndex;
			if (olen > outstandingBytes) {
				xprintf("Decoded %d base64 bytes exceed outstanding\n", olen, outstandingBytes);
				snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: Decoded %d base64 bytes", olen);
			}
			else {
				memcpy(m_binaryBuf + m_binaryBufIndex, decodedBinary, olen);
				m_binaryBufIndex += olen;
				outstandingBytes = m_expectedBytes - m_binaryBufIndex;
				if (outstandingBytes == 0) {
					// Finished
					xprintf("Decoded %d base64 bytes. Complete.\n", olen);
					snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Decoded %d base64 bytes. Complete.", olen);

				    // Perform a callback so i2c_slave_commands.c knows the data is complete
				    if (commandsCallback != NULL) {
				    	commandsCallback(AI_PROCESSOR_MSG_TX_BINARY_COMPLETE, NULL, m_expectedBytes);
				    }

					// Perform a callback so the client knows the data is complete
					if (clientCallback != NULL) {
						clientCallback(AI_PROCESSOR_MSG_TX_BINARY_COMPLETE, NULL, m_expectedBytes);
					}
				}
				else {
					xprintf("Decoded %d base64 bytes. %d outstanding\n", olen, outstandingBytes);
					snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Decoded %d base64 bytes. %d outstanding.",
							olen, outstandingBytes);
				}
			}
		}
		else {
			snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: Base64 decode error %d", ret);
		}
	}
	// The response message is now in returnMessage[]
	// We now wait for the I2C read from the WW130 to return the message
}


/**
 * The app has sent us binary data, and probably expects a response.
 *
 * Various checks are made to ensure the data can be appended to the final destination buffer.
 * If OK the message is added to that buffer, and pointers etc updated.
 *
 * Whatever happens, a response string is prepared and will be sent later.
 * when the I2C master requests a string to be returned.
 *
 * @param message - binary data sent by the app
 */
static void processBinary(uint8_t * message, uint16_t numBytes ) {
	int16_t outstandingBytes;

	// Clear the return message
	memset(i2c_slave_if_returnMessage, 0, I2C_SLAVE_IF_PAYLOAD_SIZE);

	// Now try to add it to the binary buffer
	outstandingBytes = m_expectedBytes - m_binaryBufIndex;
	if (numBytes > outstandingBytes) {
		xprintf("%d bytes exceed outstanding\n", numBytes, outstandingBytes);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: too many bytes %d", numBytes);
	}
	else {
		memcpy(m_binaryBuf + m_binaryBufIndex, message, numBytes);
		m_binaryBufIndex += numBytes;
		outstandingBytes = m_expectedBytes - m_binaryBufIndex;
		if (outstandingBytes == 0) {
			// Finished
			xprintf("Added %d bytes. Complete.\n", numBytes);
			snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Added %d bytes. Complete.", numBytes);

			// Perform a callback so i2c_slave_commands.c knows the data is complete
		    if (commandsCallback != NULL) {
		    	commandsCallback(AI_PROCESSOR_MSG_TX_BINARY_COMPLETE, NULL, m_expectedBytes);
		    }

			// Perform a callback so the client knows the data is complete
			if (clientCallback != NULL) {
				clientCallback(AI_PROCESSOR_MSG_TX_BINARY_COMPLETE, NULL, m_expectedBytes);
			}
		}
		else {
			xprintf("Added %d bytes. %d outstanding\n", numBytes, outstandingBytes);
			snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Added %d bytes. %d outstanding.",
					numBytes, outstandingBytes);
		}
	}

	// The response message is now in returnMessage[]
	// We now wait for the I2C read from the WW130 to return the message
}


/**
 * BLE app asks us to prepare to receive binary data.
 * Command is: ' AI tx binary <numBytes>'
 *
 * This routine extracts the expected number of bytes, checks the size if OK, and if so,
 * prepares buffer pointers etc to receive the binary data.
 *
 * The actual data transfer follows
 */
static void processTxBinary(char * parameter) {
	uint16_t expectedBytes;

	expectedBytes = atoi(parameter);
	if (expectedBytes == 0) {
		xprintf("App sent 'tx binary ' - length missing\n");
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: no parameter");
		m_expectedBytes = 0;
	}
	else if (expectedBytes > m_binaryBufSize) {
		xprintf("App sent 'tx binary ' - but %d too large for buffer (%d)\n", expectedBytes, m_binaryBufSize);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: no room");
		m_expectedBytes = 0;
	}
	else {
		xprintf("App sent 'tx binary ' - expecting %d bytes\n", expectedBytes);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Expecting %d bytes", expectedBytes);
		m_binaryBufIndex = 0;
		//memset(m_binaryBuf, 0, m_binaryBufSize);	// Not necessary?
		m_expectedBytes = expectedBytes;
	}
}

/**
 * BLE app asks us to prepare to send binary data.
 * Command is: ' AI rx binary <numBytes>'
 *
 * This routine extracts the expected number of bytes, checks the size if OK, and if so,
 * prepares buffer pointers etc to send the binary data.
 *
 * The actual data transfer follows. Typically this will take several smaller transfers
 *
 * @param parameter = string containing the number of bytes to transfer (decimal)
 */
static void processRxBinary(char * parameter) {
	uint16_t expectedBytes;

	expectedBytes = atoi(parameter);

	if (expectedBytes == 0) {
		xprintf("App sent 'rx binary ' - length missing\n");
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: no parameter");
		m_expectedBytes = 0;
	}
	else if (expectedBytes > m_binaryBufSize) {
		xprintf("App sent 'rx binary ' - but %d is larger than the available buffer (%d)\n", expectedBytes, m_binaryBufSize);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: too large");
		m_expectedBytes = 0;
	}
	else {
		xprintf("App sent 'rx binary ' - expecting to send %d bytes\n", expectedBytes);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Expecting to send %d bytes", expectedBytes);
		m_binaryBufIndex = 0;
		m_expectedBytes = expectedBytes;
	}
}

/**
 * BLE app asks to send binary data.
 * Command is: ' AI rx * <numBytes>'
 *
 * This routine extracts the expected number of bytes, checks the size if OK, and if so,
 * prepares buffer pointers etc to send the binary data.
 *
 * @param parameter = string containing the number of bytes to transfer (decimal) in this transfer
 */
static void processRequestBinary(char * parameter) {
	uint16_t requestedBytes;
	uint16_t outstandingBytes;

	requestedBytes = atoi(parameter);
	outstandingBytes = m_expectedBytes - m_binaryBufIndex;

	if (requestedBytes == 0) {
		xprintf("App sent 'rx * ' - length missing\n");
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: no parameter");
		m_requestedBytes = 0;
	}
	else if (requestedBytes > outstandingBytes) {
		xprintf("App sent 'rx * ' - but %d is larger than outstanding bytes (%d)\n", requestedBytes, outstandingBytes);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: too large");
		m_requestedBytes = 0;
	}
	else if (requestedBytes > I2C_SLAVE_IF_PAYLOAD_SIZE) {
		// won't fit in the transfer buffer
		xprintf("App sent 'rx * ' - but %d is larger than transfer buffer (%d)\n", requestedBytes, I2C_SLAVE_IF_PAYLOAD_SIZE);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: too large");
		m_requestedBytes = 0;
	}
	else {
		xprintf("App sent 'rx * ' - sending %d bytes\n", requestedBytes);
		snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Sending %d bytes", requestedBytes);
		m_requestedBytes = requestedBytes;
		// Now wait for the AI_PROCESSOR_MSG_RX_BINARY command
	}
}


static void processRequestBase64(char * parameter) {
	// placeholder
	snprintf(i2c_slave_if_returnMessage, I2C_SLAVE_IF_PAYLOAD_SIZE, "Err: not yet implemented");
	m_requestedBytes = 0;
}

/**
 * Timer callback after a delay to assert PA0
 */
static void pa0TimeoutCb(void *data) {

	hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);
	hx_drv_gpio_set_input(AON_GPIO0);

	// The next commands prepare PA0 to be an interrupt input
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt
}

/****************************** Exported Function Definitions ************************/

/**
 * Initialise the I2C slave peripheral.
 *
 * SDA = PA3 pin
 * SCL = PA2 pin
 *
 * Defines callbacks for data TX and RX, and error.
 * Defines the I2C slave address
 *
 */
void i2c_slave_if_init(void * clientCb, void * commandsCb) {
	uint8_t gpio_value;

	clientCallback = clientCb;
	commandsCallback = commandsCb;

	// PA0 can either be an output to the WW130 - driven as 0 - or an input.
	// When PA0 is an input then it can be enabled as an interrupt. This allows the WW130 to interrupt this board
	// by driving PA0 low. That is - the same PA0 signal can be used by either side to interrupt the other.

	// Initialise PA0 as an input. Expect a pull-up to take it high.
	// This device can then set PA0 to an output at logic 0 to change the state of the pin.
    hx_drv_gpio_set_input(AON_GPIO0);
    // For reasons I don't understand, we must use SCU_PA0_PINMUX_AON_GPIO0_2 not SCU_PA0_PINMUX_AON_GPIO0_0
    //hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_2, 1);

	// Set PA0 PULL_UP
    SCU_PAD_PULL_LIST_T pad_pull_cfg;
    hx_drv_scu_get_all_pull_cfg(&pad_pull_cfg);
	pad_pull_cfg.pa0.pull_en = SCU_PAD_PULL_EN;
	pad_pull_cfg.pa0.pull_sel = SCU_PAD_PULL_UP;
    hx_drv_scu_set_all_pull_cfg(&pad_pull_cfg);

	hx_drv_gpio_get_in_value(AON_GPIO0, &gpio_value);

	// The next commands prepare PA0 to be an interrupt input
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_cb_register(AON_GPIO0, aon_gpio0_cb);	// define ISR
	hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_FALLING);	// only when PA0 goes low
	//hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_BOTH);	// When PA0 goes low, then when it goes high
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt

	xprintf("Initialise PA0 (AON_GPIO0) as input. Read %d\n", gpio_value);

	// Initialise the I2C slave functionality
	initI2CSlave();
}

/**
 * Uninitilise I2C slave
 */
void i2c_slave_if_deinit(void) {
    uint32_t i2c_s_id = USE_DW_IIC_SLV_0;

    hx_drv_i2cs_deinit(i2c_s_id);
}

/**
 * Send binary data.
 *
 * The parameters should have been set up already!
 */
static void sendBinaryData(void) {
	uint16_t length;

	xprintf("Preparing to send %d binary bytes (plus %d control bytes)\n",
			m_requestedBytes, I2C_SLAVE_IF_CTRL_BYTES);

	// This test should have happened earlier, but just in case...
	if (m_requestedBytes > I2C_SLAVE_IF_PAYLOAD_SIZE) {
		// won't fit in the buffer
		xprintf("%d is too much. Truncated to %d\n", m_requestedBytes, I2C_SLAVE_IF_PAYLOAD_SIZE);
		length = I2C_SLAVE_IF_PAYLOAD_SIZE;	// truncate
	}
	else {
		length = m_requestedBytes;
	}

	tx_buffer[0] = AI_PROCESSOR_MSG_RX_BINARY;	//"RX" is from the point of view of the master
	tx_buffer[1] = (uint8_t) (length & 0xff);	// send length as a LE value
	tx_buffer[2] = (uint8_t) (length >> 8);

	// Add the (next part of) the payload
	memcpy(&tx_buffer[I2C_SLAVE_IF_CTRL_BYTES], m_binaryBuf + m_binaryBufIndex, length);
	i2cs_write_enable(I2C_SLAVE_IF_CTRL_BYTES + length);	// This initiates the actual I2C transfer
}

/**
 * Request to send this via I2C
 * Assumes the message has already been set up in tx_buffer
 */
void i2cs_slave_if_send_string(char * message) {
	uint16_t length;

	// This ensures that 'length' is at maximum 1 less than the buffer size
	// It has the effect of truncating long strings
	length = strnlen(message, I2C_SLAVE_IF_PAYLOAD_SIZE - 1);
	// This ensures that whatever we have is terminated with 0. Also, length includes the '\0'
	message[length++] = '\0';

	// This is what is returned to the WW130
	xprintf("Returning a %d byte response: ", length);
	P_CYAN;
	xprintf("'%s'\n", message);
	P_WHITE;

	// Prepend any message type bytes
	tx_buffer[0] = AI_PROCESSOR_MSG_RX_STRING;	//"RX" is from the point of view of the master
	tx_buffer[1] = (uint8_t) (length & 0xff);	// send length as a LE value
	tx_buffer[2] = (uint8_t) (length >> 8);

	memcpy(&tx_buffer[I2C_SLAVE_IF_CTRL_BYTES], message, length);
	i2cs_write_enable(I2C_SLAVE_IF_CTRL_BYTES + length);
}

//
/**
 * Toggle inter-processor interrupt pin to wake WW130
 *
 * This routine sets the pin to an output, low.
 * It then sets a timer which restores it to input, high.
 *
 * TODO consider testing for miniumum and maximum pulseWidths
 * Experimentally it looks like this might need to be low for > 90ms - say 200ms?
 * I don't know why this is so slow...
 *
 * @param pulseWidth = delay in ms
 */
void i2cs_slave_wake_master(uint32_t pulseWidth) {
	TIMER_CFG_T timer_cfg;

	// Disable interrupt so we don't interrut ourselves
	hx_drv_gpio_set_int_enable(AON_GPIO0, 0);	// 0 means disable interrupt

	// Sets PA0 as an output and drive low, then delay, then high, then set as an input
	hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);

	// Set a timer to take the pin high again
	timer_cfg.period = pulseWidth;
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_CPU;
	timer_cfg.state = TIMER_STATE_DC;

    hx_drv_timer_cm55m_stop();
	hx_drv_timer_cm55m_start(&timer_cfg, (Timer_ISREvent_t) pa0TimeoutCb);
}


/**
 * Prepare internal pointers etc so that incoming binary data can be placed
 * in the ultimate destination buffer. The buffer is also used for outgoing messages.
 *
 * This is called by the client routines, to define a buffer to receive incoming binary data.
 * The buffer is also used for outgoing messages.
 *
 * TODO is there any use in defining two buffers, for incoming and outgoing?
 *
 * This process assumes that large binary data transfers occur in multiple, smaller, transfers
 * from the BLE device via the WW130.
 *
 * @param binaryBuffer = pointer to buffer to contain the binary data
 * @param bufSize = size of that buffer
 */
void i2cs_slave_if_binaryBufferInit(uint8_t * binaryBuffer, uint16_t bufSize) {
	m_binaryBuf = binaryBuffer;
	m_binaryBufIndex = 0;
	m_binaryBufSize = bufSize;
}

/**
 * Utility code to report on whether we are in ISR
 *
 * See https://devzone.nordicsemi.com/f/nordic-q-a/69500/what-is-the-easiest-way-to-find-if-certain-part-of-code-is-executed-from-isr
 */
bool i2cs_slave_if_inISR(void) {
	if ( SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk ) {
		//NRF_LOG_INFO("inside  ISR");
		return true;
	}
	else {
		// NRF_LOG_INFO("not inside ISR (Thread mode)");
		return false;
	}
// Alternatively...
//    if (current_int_priority_get() == APP_IRQ_PRIORITY_THREAD) {
//        return false;
//    }
//    else {
//        return true;;
//    }
}

// A utility for printing the contents of a buffer
void i2cs_slave_if_printBuffer(uint8_t * buff, uint16_t length) {
    uint8_t lineBuff[IN_LINE_PRINT_CNT + 1];	  // +1 for '\0'

    // Print them, 16 at a time.
    for (uint16_t addr = 0; addr < length; addr += IN_LINE_PRINT_CNT)  {
    	memcpy(lineBuff, buff + addr, IN_LINE_PRINT_CNT);
        lineBuff[IN_LINE_PRINT_CNT] = '\0';

        xprintf("%04x: ", addr);

        for (uint8_t i = 0; i < IN_LINE_PRINT_CNT; i++)  {
        	xprintf("%02x ", lineBuff[i]);
            if (i == 7) {
            	// Extra space to separate 1st and 2nd 8 bytes
            	xprintf(" ");
            }
            if (!isprint((int)lineBuff[i]))  {
            	// replace the non-printing character in the buffer with '.'
                lineBuff[i] = '.';
            }
        }
        // Now the string version
        xprintf("%s\n", lineBuff);
    }
}
