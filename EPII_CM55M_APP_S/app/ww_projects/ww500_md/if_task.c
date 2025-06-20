/*
 * if_task.c
 *
 *  Created on: 12 Aug 2024
 *      Author: CGP
 *
 * FreeRTOS task
 *
 * This task handles communication with the WW130. This involves setting up and operating as an I2C slave,
 * and operating PA0 as a bidirectional interrupt pin.
 *
 * This uses code from ww130_test,
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
#include "if_task.h"
#include "fatfs_task.h"
#include "i2c_comm.h"

#include "WE2_debug.h"

#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"

#include "crc16_ccitt.h"
#include "ww500_md.h"

/*************************************** Definitions *******************************************/

// Uncomment to allow testing of the interprocessor interrupt ppin
#define TEST_INT_PULSE

#define EVT_I2CS_0_SLV_ADDR     0x62

#define IFTASK_QUEUE_LEN   		10

// Time in ms before giving up on I2C master read
// Note: when uploading files using the txfile command, some packets were lost:
// This timer timed out, and the ML62BA saw two interprocessor interrupts and gave a 'busy' error.
// See if this is resolved by increasing the timeout
//#define MISSINGMASTERTIME	300
#define MISSINGMASTERTIME	1000

#define DBG_EVT_IICS_CMD_LOG 1
#if DBG_EVT_IICS_CMD_LOG
    #define dbg_evt_iics_cmd(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cmd(fmt, ...)
#endif

#define DBG_EVT_IICS_CALLBACK_LOG 1
#if DBG_EVT_IICS_CALLBACK_LOG
    #define dbg_evt_iics_cb(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cb(fmt, ...)
#endif

#define TESTFILELEN (20)

/*************************************** Local Function Declarations *****************************/

static void vIfTask(void *pvParameters);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T  handleEventForIdle(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStateI2CRx(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStateI2CTx(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStateI2CSlaveTx(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStateI2CSlaveRx(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStatePA0(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStateDiskOp(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T  flagUnexpectedEvent(APP_MSG_T rxMessage);

static uint16_t app_i2ccomm_init();
// Functions relating to interprocessor interrupts
static void interprocessor_interrupt_cb(uint8_t group, uint8_t aIndex);
static void interprocessor_interrupt_init(void);
static void interprocessor_interrupt_assert(void);
static void interprocessor_interrupt_negate(void);

// Three callbacks from the I2C module - execute in ISR context
static void i2csTxDoneEvent(void *param);
static void i2csRxReadyEvent(void *param);
static void i2csErrorEvent(void *param);

// Three functions which convert the I2C module callbacks into non-ISR context calls.
static void i2cTransmissionComplete(void);
static void i2cRxDataReady(void);
static void i2cError(void);

static void i2ccomm_write_enable(uint8_t * message, aiProcessor_msg_type_t messageType, uint16_t length);
static void clear_read_buf_header(void);

static void missingMasterExpired(xTimerHandle pxTimer);

//static void deferPA0Pulse(uint32_t pulseWidth);

static void sendI2CMessage(uint8_t * data, aiProcessor_msg_type_t messageType, uint16_t length);

// I2C slave address and callbacks - used for initialisation only
I2CCOMM_CFG_T gI2CCOMM_cfg = {
        EVT_I2CS_0_SLV_ADDR,
        i2csTxDoneEvent,
        i2csRxReadyEvent,
        i2csErrorEvent
};

/*************************************** Local variables *******************************************/

static APP_WAKE_REASON_E woken;

// which I2C slave instance to use
static USE_DW_IIC_SLV_E iic_id;

SemaphoreHandle_t xI2CTxSemaphore;
SemaphoreHandle_t xIfCanSleepSemaphore;

// This is the handle of the task
TaskHandle_t 	ifTask_task_id;
QueueHandle_t     xIfTaskQueue;

extern QueueHandle_t     xCliTaskQueue;
extern QueueHandle_t     xFatTaskQueue;


volatile APP_IF_STATE_E if_task_state = APP_IF_STATE_UNINIT;

#ifdef TEST_INT_PULSE
// Timer for pulsing interprocessor interrupt pin to MKL62BA - test only
xTimerHandle timerInterruptPulse;

static void onInterruptTimerExpired(xTimerHandle pxTimer);

// Time in ms that the interprocessor interrupt pin is low
#define INTERRUPTPULSEWIDTH	120

#endif	// TEST_INT_PULSE

// Timer to case case if I2C master does not read our data
xTimerHandle timerHndlMissingMaster;

//I2C incoming and outgoing buffers
uint8_t gWrite_buf[WW130_MAX_WBUF_SIZE];
uint8_t gRead_buf[WW130_MAX_RBUF_SIZE];

// This is used to save a message we can't deal with in the current state. Re-issued when we return to IDLE
static APP_MSG_T savedMessage;

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char * ifTaskStateString[APP_IF_STATE_NUMSTATES] = {
		"Uninitialised",
		"Idle",
		"I2C RX State",
		"I2C TX State",
		"I2C TX State (slave)",
		"I2C RX State (slave)",
		"PA0 State",
		"Disk Op State",
};

// Strings for expected messages. Values must match Messages directed to IF Task in app_msg.h
// If I define xxx_FIRST and xx_LAST a I have done, then this gives the size of the array:
const char* ifTaskEventString[APP_MSG_IFTASK_LAST - APP_MSG_IFTASK_FIRST] = {
		"I2C Rx",
		"I2C Tx",
		"I2C Error",
		"String Response",
		"String Continues",
		"Binary Response",
		"Binary Continues",
		"MKL62BA Interrupt In",
		"MKL62BA Interrupt Out",
		"MKL62BA Int Timer",
		"MM Timer",
		"Disk Write Complete",
		"Disk Read Complete",
		"Message to Master",
		"Inactivity",
		"Awake"
};

// Strings for the events - must align with aiProcessor_msg_type_t entries
const char *cmdString[] = {
		"None",
		"Received string",
		"Received base64",
		"Received binary",
		"Read string",
		"Read base64",
		"Read binary"	};

// This is a first message sent when we exit reset (cold or warm).
const char * firstMessage = "Wake";

// This is the final string sent before we enter DPD
const char * lastMessage = "Sleep";

//bool firstMessageSent = false;
bool lastMessageSent = false;

/*********************************** I2C Local Function Definitions ************************************************/

/**
 * I2C slave callback - called when the Master has read our I2C data.
 *
 * This in in the ISR context. We pass an event to the comm_task loop which then
 * calls evt_i2ccomm_tx_cb() in the task's main thread.
 */
static void i2csTxDoneEvent(void *param) {
	//HX_DRV_DEV_IIC *iic_obj = param;
	//HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

	// Stop the timer in case the master does not read our data
	if (xTimerStop(timerHndlMissingMaster, 0) != pdPASS) {
		configASSERT(0);	// TODO add debug messages?
	}

	//send_msg.msg_data = iic_info_ptr->slv_addr;
	send_msg.msg_data = 0;
	send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_TX_DONE;

	xQueueSendFromISR( xIfTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
	if( xHigherPriorityTaskWoken )  {
		taskYIELD();
	}
}

/**
 * I2C slave callback - when we receive data from the master (MKL62BA).
 *
 * This in in the ISR context.
 *
 * We pass an  APP_MSG_IFTASK_I2CCOMM_RX event to the iftask loop
 * which then calls evt_i2ccomm_rx_cb in the task's main thread.
 *
 * The messages itself remains in gRead_buf[]
 */
static void i2csRxReadyEvent(void *param) {
	APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

	send_msg.msg_data = 0;
	send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_RX_READY;

	//dbg_printf(DBG_LESS_INFO, "I2C RX ISR. Send to ifTask 0x%x\r\n", send_msg.msg_event);

	xQueueSendFromISR( xIfTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
	if( xHigherPriorityTaskWoken )  {
		taskYIELD();
	}
}

/**
 * I2C error callback
 *
 * This in in the ISR context. We pass an event to the comm_task loop which then
 * calls evt_i2ccomm_0_err_cb() in the task's main thread.
 *
 * A possible error is 'DEV_IIC_ERR_TX_DATA_UNREADY' = 7
 * which happens if the I2C master tries to read before the slave has data to send
 * i.e. before we call i2ccomm_write_enable()
 */
static void i2csErrorEvent(void *param) {
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t data;

	data = iic_info_ptr->err_state;

    send_msg.msg_data = data;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_ERR;

    if (data == DEV_IIC_ERR_TX_DATA_UNREADY) {
    	// I2C master reads before we have data to send
        dbg_printf(DBG_LESS_INFO, "I2C Error: data unready. Sending 0x%x to ifTask\r\n", send_msg.msg_event);
    }
    else {
    dbg_printf(DBG_LESS_INFO, "I2C Error %d. Sending 0x%x to ifTask\r\n", data, send_msg.msg_event);
    }

    xQueueSendFromISR( xIfTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
    if( xHigherPriorityTaskWoken )  {
    	taskYIELD();
    }
}

/**
 * Slave I2C transmission to master is complete.
 *
 * Called from within the ifTask loop in response to the interrupt callback in i2cs_cb_tx()
 *
 * This readies us for a new command from the master
 */
static void i2cTransmissionComplete(void) {

	dbg_evt_iics_cmd("I2C transmission complete.\n");

	// Release the I2C semaphore
	xSemaphoreGive(xI2CTxSemaphore);

    // Prepare for the next incoming message
    clear_read_buf_header();
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, WW130_MAX_RBUF_SIZE);
}

/**
 * Called when the I2C master sends us a message
 *
 * Called from within the ifTask loop in response to the interrupt callback in i2cRxDataReady()
 *
 * Messages of type AI_PROCESSOR_MSG_TX_STRING are passed to the CLI task for parsing and executing
 */
static void i2cRxDataReady(void) {
	uint8_t feature;
	uint8_t cmd;
	uint16_t length;
	uint8_t * payload;
	bool crcOK;
	APP_MSG_T send_msg;

	feature = gRead_buf[I2CFMT_FEATURE_OFFSET];
	cmd 	= gRead_buf[I2CFMT_COMMAND_OFFSET];
	length 	= (gRead_buf[I2CFMT_PAYLOADLEN_LSB_OFFSET]) + ((gRead_buf[I2CFMT_PAYLOADLEN_MSB_OFFSET] ) << 8);
	payload = &gRead_buf[I2CFMT_PAYLOAD_OFFSET];

	XP_YELLOW;
	dbg_evt_iics_cmd("\n");
	dbg_evt_iics_cmd("Received I2C message.\n");

	XP_LT_GREY;
	// Let's print the message
	printf_x_printBuffer(gRead_buf, (I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE ));
	XP_WHITE;

	crcOK = crc16_ccitt_validate(gRead_buf, I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE );

	// Now overwrite the checksum with 0 so the payload becomes a string
	gRead_buf[I2CCOMM_HEADER_SIZE + length] = '\0';

	if  ((feature != WW130_CMD_FEATURE) || (cmd >= AI_PROCESSOR_MSG_END)) {
		xprintf("Received feature=%d cmd=%d length=%d \n", feature, cmd, length);
		xprintf("Command is not recognised\n");
		return;
	}

//	xprintf("Received feature=%d cmd=%d (%s) length=%d '%s' CRC %s\n",
//			feature, cmd, cmdString[cmd], length, (char * ) payload, crcOK ? "OK" : "failed");

	// Check the CRC
	if(!crcOK) {
		// TODO this is probably not adequate. Check!
		return;
	}

	// Now let's see if we can make sense of the message
	switch (cmd) {
	case AI_PROCESSOR_MSG_TX_STRING:
		// I2C master has sent a string. We assume it is a command to execute
		xprintf("MKL62BA command received: '%s'\n", (char * ) payload);
		// Now see if we can send this to the CLI task for processing
		send_msg.msg_event = APP_MSG_CLITASK_RXI2C;
		send_msg.msg_data = (uint32_t) payload;

		if(xQueueSend( xCliTaskQueue , (void *) &send_msg , __QueueSendTicksToWait) != pdTRUE) {
			dbg_printf(DBG_LESS_INFO, "send_msg=0x%x fail\r\n", send_msg.msg_event);
		}
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

    // Prepare I2C component for the next incoming message
	clear_read_buf_header();
	hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, WW130_MAX_RBUF_SIZE);
}

/**
 * Slave I2C transmission to master has an error.
 *
 * Called from within the ifTask loop in response to the interrupt callback in i2cs_cb_err()
 *
 * This readies us for a new command from the master
 */
static void i2cError(void) {
	XP_LT_RED;
	dbg_evt_iics_cmd("\n");
	dbg_evt_iics_cmd("I2C error\n");
	XP_WHITE;

    // Prepare for the next incoming message
    clear_read_buf_header();
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, WW130_MAX_RBUF_SIZE);
}

/**
 * Called by the client when it has a message to send to the I2C master.
 *
 * If a string is being sent, the trailing '\0' will have been added.
 *
 * Calls hx_lib_i2ccomm_enable_write() and response arrives in i2cs_cb_rx()
 *
 * @param message = pointer to the buffer containing the message
 * @param messageType =
 * @param length = number of bytes to send
 */
static void i2ccomm_write_enable(uint8_t * message, aiProcessor_msg_type_t messageType, uint16_t length) {
    uint16_t checksum;
    I2CCOMM_ERROR_E ret;

    if (length > WW130_MAX_PAYLOAD_SIZE) {
    	length = WW130_MAX_PAYLOAD_SIZE;
    }

    // prepare write buffer for write process
    gWrite_buf[I2CFMT_FEATURE_OFFSET] = WW130_CMD_FEATURE;
    gWrite_buf[I2CFMT_COMMAND_OFFSET] = messageType; // master is receiving a string
    gWrite_buf[I2CFMT_PAYLOADLEN_LSB_OFFSET] = length & 0xff;
    gWrite_buf[I2CFMT_PAYLOADLEN_MSB_OFFSET] = (length >> 8) & 0xff;

    // Now copy the payload to the right place
	memcpy(&gWrite_buf[I2CFMT_PAYLOAD_OFFSET], message, length);

	crc16_ccitt_generate(gWrite_buf, I2CFMT_PAYLOAD_OFFSET + length, &checksum);
	// CRC is BE
    gWrite_buf[I2CCOMM_HEADER_SIZE + length] = (checksum >> 8) & 0xff;
    gWrite_buf[I2CCOMM_HEADER_SIZE + length + 1] = checksum & 0xff;

    dbg_evt_iics_cmd("Sending %d payload bytes. Checksum generated: 0x%04x Sending %d bytes in total\n",
    		length, checksum, (I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE));

    hx_CleanDCache_by_Addr((void *) gWrite_buf, I2CCOMM_MAX_RBUF_SIZE);

    // for debugging, print the buffer
    XP_LT_GREY;
    printf_x_printBuffer((uint8_t *) gWrite_buf, I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE);
    XP_WHITE;

    // non-blocking I2C transmit. Expect an interrupt in i2cs_cb_tx() soon.
    ret = hx_lib_i2ccomm_enable_write(iic_id, gWrite_buf);

    if(ret != I2CCOMM_NO_ERROR){
    	xprintf("I2C write error %d\n", ret);
    }

	// Start a timer in case the master does not read our data
	if (xTimerStart(timerHndlMissingMaster, 0)!= pdPASS) {
		configASSERT(0);	// TODO add debug messages?
	}
}

/*************************************** Local Function Definitions *****************************/

/**
 * Implements state machine when in APP_IF_STATE_IDLE
 *
 */
static APP_MSG_DEST_T handleEventForIdle(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;
    uint32_t length;

	event = rxMessage.msg_event;
	data = rxMessage.msg_data;

	length = rxMessage.msg_parameter;
	if (length > WW130_MAX_PAYLOAD_SIZE) {
		length = WW130_MAX_PAYLOAD_SIZE;
	}

	switch (event) {

	case APP_MSG_IFTASK_I2CCOMM_RX_READY:
		// Here when I2C message arrived
		if_task_state = APP_IF_STATE_I2C_RX;
		// Read and parse the incoming data. Messages of type  AI_PROCESSOR_MSG_TX_STRING are passed to the CLI task for parsing and executing
		i2cRxDataReady();
		break;

	case APP_MSG_IFTASK_I2CCOMM_ERR:
		// Unexpected. Could be caused by WW130 doing an unexpected I2C read. Ignore.
		i2cError();
		break;


	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN:
		// Not used at the moment
		break;

//	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_RESPONSE:
//	case APP_MSG_IFTASK_I2CCOMM_CLI_BINARY_RESPONSE:
//		// This could happen if the response we want to send to the I2C master is ready too late,
//		// and the master has attempted to read before data is ready, in which case we get an I2C error
//		// and the state machine returns us to IDLE.
//		// So just ignore this, and stay in IDLE.
//		break;


	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_RESPONSE:
	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_CONTINUES:
		// Here if there are several lines from the CLI response
		// AI_PROCESSOR_MSG_RX_STRING means the WW130 will see a "receive string" message
		sendI2CMessage((uint8_t *) data, AI_PROCESSOR_MSG_RX_STRING, (uint16_t) length);
		if_task_state = APP_IF_STATE_I2C_TX;
		break;


	case APP_MSG_IFTASK_I2CCOMM_CLI_BINARY_RESPONSE:
	case APP_MSG_IFTASK_I2CCOMM_CLI_BINARY_CONTINUES:
		// Return this binary data to the WW130
		// AI_PROCESSOR_MSG_RX_BINARY means the WW130 will see a "receive binary" message
		sendI2CMessage((uint8_t *) data, AI_PROCESSOR_MSG_RX_BINARY, (uint16_t) length);
		if_task_state = APP_IF_STATE_I2C_TX;
		break;


	case APP_MSG_IFTASK_MSG_TO_MASTER:
		// Here when this processor initiates communications with MKL62BA
		sendI2CMessage((uint8_t *) data, AI_PROCESSOR_MSG_RX_STRING, (uint16_t) length);
		// TODO - think carefully whether we need this state...
		//if_task_state = APP_IF_STATE_I2C_SLAVE_TX;
		if_task_state = APP_IF_STATE_I2C_TX;
		break;

	case APP_MSG_IFTASK_AWAKE:
		// We have just woken, so send a message to the BLE processor

		//firstMessageSent = true;
		sendI2CMessage((uint8_t *) firstMessage, AI_PROCESSOR_MSG_RX_STRING, (uint16_t) strlen(firstMessage));
		if_task_state = APP_IF_STATE_I2C_TX;
		break;

	case APP_MSG_IFTASK_INACTIVITY:
		// We are being asked to send a message to the BLE processor
		// Then set a semaphore to say it is done and we can sleep

		lastMessageSent = true;
		sendI2CMessage((uint8_t *) lastMessage, AI_PROCESSOR_MSG_RX_STRING, (uint16_t) strlen(lastMessage));
		if_task_state = APP_IF_STATE_I2C_TX;

		break;

#ifdef TEST_INT_PULSE
	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_OUT:
		// Interrupt the MKL62BA by driving this pin low. At present the WW130 only responds when the pin goes high again,
		// after a delay.
		interprocessor_interrupt_assert();

		// start a timer to undo this (take the pin high). This generates a APP_MSG_IFTASK_I2CCOMM_PA0_TIMER message
		//dbg_printf(DBG_LESS_INFO, "DEBUG: change period to %d\r\n", data);
		if (data > 0) {
			if( xTimerChangePeriod(timerInterruptPulse, (data / portTICK_PERIOD_MS), 100 ) != pdPASS ) {
				configASSERT(0);	// TODO add debug messages?
			}
			if (xTimerStart(timerInterruptPulse, 0)!= pdPASS) {
				configASSERT(0);	// TODO add debug messages?
			}
		}

		// TODO think about this state...
		if_task_state = APP_IF_STATE_PA0;
		break;

#endif // TEST_INT_PULSE

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_IF_STATE_I2C_RX
 *
 * This state is entered when an I2C message arrives from the MKL62BA
 * (initiated by MKL62BA)
 */
static APP_MSG_DEST_T  handleEventForStateI2CRx(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;
	uint32_t length;

	// Do this to stop compiler warnings for unused variables:
	//(void) event;

	event = rxMessage.msg_event;
	data = rxMessage.msg_data;
	length = rxMessage.msg_parameter;
	if (length > WW130_MAX_PAYLOAD_SIZE) {
		length = WW130_MAX_PAYLOAD_SIZE;
	}

	switch (event) {
	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_RESPONSE:
		// Return this string to the WW130
		// AI_PROCESSOR_MSG_RX_STRING means the WW130 will see a "receive string" message
		sendI2CMessage((uint8_t *) data, AI_PROCESSOR_MSG_RX_STRING, (uint16_t) length);
		if_task_state = APP_IF_STATE_I2C_TX;
		// The WW130 should read the message then we will get a APP_MSG_IFTASK_I2CCOMM_TX event
		// Then a call to evt_i2ccomm_tx_cb() which releases the semaphore
		break;

	case APP_MSG_IFTASK_I2CCOMM_CLI_BINARY_RESPONSE:
		// Return this binary data to the WW130
		// AI_PROCESSOR_MSG_RX_BINARY means the WW130 will see a "receive binary" message
		sendI2CMessage((uint8_t *) data, AI_PROCESSOR_MSG_RX_BINARY, (uint16_t) length);
		if_task_state = APP_IF_STATE_I2C_TX;
		break;

	case APP_MSG_IFTASK_I2CCOMM_ERR:
		if_task_state = APP_IF_STATE_IDLE;
		i2cError();
		break;

	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_OUT:
		// This can happen if the CLI command processor sees "int 200" or similar.
		// We shouln't execute this right away, as we still have to send the I2C response to the ww130
		XP_BROWN;
		xprintf("Deferring event 0x%04x\n", event);
		XP_WHITE;
		savedMessage = rxMessage;
		break;

	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN:
		// Not used at the moment
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_IF_STATE_I2C_TX
 *
 * This is the state when the I2C interface is transmitting to the MKL62BA
 * (a response initiated by a MKL62BA request)
 */
static APP_MSG_DEST_T  handleEventForStateI2CTx(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_IFTASK_I2CCOMM_TX_DONE:
		i2cTransmissionComplete();
		if_task_state = APP_IF_STATE_IDLE;

		if (lastMessageSent) {
			// special case just before entering DPD.

			xprintf("DEBUG: giving semaphore\n");
			// The semaphore lets the Image Task enter DPD
			xSemaphoreGive(xIfCanSleepSemaphore);
		}
		break;

	case APP_MSG_IFTASK_I2CCOMM_MM_TIMER:
		// Master failed to respond to our attempt to send I2C data
		XP_LT_RED;
		xprintf("I2C master did not read our I2C message\n");
		XP_WHITE;

		i2cTransmissionComplete();
		if_task_state = APP_IF_STATE_IDLE;
		break;

	case APP_MSG_IFTASK_I2CCOMM_ERR:
		if_task_state = APP_IF_STATE_IDLE;
		i2cError();
		break;

	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN:
		// Not used at the moment
		break;

	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_RESPONSE ... APP_MSG_IFTASK_I2CCOMM_CLI_BINARY_CONTINUES:
		// This could happen if the ifTask is still sending a previous message
		// and APP_MSG_IFTASK_I2CCOMM_TX has not yet arrived. So save the response and process it when we return to IDLE
		XP_BROWN;
		xprintf("Deferring event 0x%04x\n", event);
		XP_WHITE;
		savedMessage = rxMessage;
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_IF_STATE_I2C_SLAVE_TX
 *
 * This is the state when the I2C interface is transmitting to the MKL62BA
 * (an exchange initiated by an HX6538 request)
 *
 * It is initiated when some other task needs to send a messages to the MKL62BA.
 *
 */
static APP_MSG_DEST_T  handleEventForStateI2CSlaveTx(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;

	switch (event) {
	case APP_MSG_IFTASK_I2CCOMM_TX_DONE:
		// I2C transmission has finished. Expecting a response from the MKL62BA soon.
		if_task_state = APP_IF_STATE_I2C_SLAVE_RX;
		i2cTransmissionComplete();
		// Starts Missing Master timer
		//evt_i2ccomm_tx_cb();
		break;

	case APP_MSG_IFTASK_I2CCOMM_MM_TIMER:
		// Missing Master timer expired. Master failed to respond to our attempt to send I2C data
		XP_LT_RED;
		xprintf("I2C master did not read our I2C message\n");
		XP_WHITE;

		i2cTransmissionComplete();
		if_task_state = APP_IF_STATE_IDLE;
		break;

	case APP_MSG_IFTASK_I2CCOMM_ERR:
		if_task_state = APP_IF_STATE_IDLE;
		i2cError();
		break;

// TODO think abot what is expected!
//	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN:
//		// Not used at the moment
//		break;
//
//	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_RESPONSE ... APP_MSG_IFTASK_I2CCOMM_CLI_BINARY_CONTINUES:
//		// This could happen if the ifTask is still sending a previous message
//		// and APP_MSG_IFTASK_I2CCOMM_TX has not yet arrived. So save the response and process it when we return to IDLE
//		XP_BROWN;
//		xprintf("Deferring event 0x%04x\n", event);
//		XP_WHITE;
//		savedMessage = rxMessage;
//		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_IF_STATE_I2C_SLAVE_RX
 *
 * This state is entered when an I2C message arrives from the MKL62BA
 * (initiated by HX6538)
 */
static APP_MSG_DEST_T  handleEventForStateI2CSlaveRx(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
//	data = rxMessage.msg_data;
//	length = rxMessage.msg_parameter;
//	if (length > WW130_MAX_PAYLOAD_SIZE) {
//		length = WW130_MAX_PAYLOAD_SIZE;
//	}

	switch (event) {

		// TODO - which event?
	case APP_MSG_IFTASK_I2CCOMM_CLI_STRING_RESPONSE:
	case APP_MSG_IFTASK_I2CCOMM_RX_READY:
		// Here when I2C message arrived
		if_task_state = APP_IF_STATE_I2C_RX;
		// Read and parse the incoming data. Messages of type  AI_PROCESSOR_MSG_TX_STRING are passed to the CLI task for parsing and executing
		i2cRxDataReady();
		break;


	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Implements state machine when in APP_IF_STATE_PA0
 *
 * This state is used to test the inter-processor interrupt
 *
 */
static APP_MSG_DEST_T  handleEventForStatePA0(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	// Do this to stop compiler warnings for unused variables:
	//(void) event;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_IFTASK_I2CCOMM_PA0_TIMER:
		interprocessor_interrupt_negate();
		if_task_state = APP_IF_STATE_IDLE;
		break;

	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN:
		// Not used at the moment
		break;

	case APP_MSG_IFTASK_I2CCOMM_ERR:
		// Unexpected. Could be caused by WW130 doing an unexpected I2C read. Ignore.
		i2cError();
		break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;

}

/**
 * Implements state machine when in APP_IF_STATE_DISK_OP
 *
 * Expect to get the results of the disk operation.
 *
 * Expected events:
 *
 * Parameters: APP_MSG_T rxMessage
 * Returns: APP_MSG_DEST_T send_msg
 */
static APP_MSG_DEST_T  handleEventForStateDiskOp(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	// No longer expecting to do disk accesses from this task...
//	case APP_MSG_IFTASK_I2CCOMM_DISK_WRITE_COMPLETE:
//		//xprintf("Res code %d\n", data);	// This is the same as fileOp.res
//		// the fileOp structure should have the results
//		xprintf("Wrote %d bytes to '%s'. Result code: %d\n",
//				fileOp.length, fileOp.fileName, fileOp.res);
//
//		if_task_state = APP_IF_STATE_IDLE;
//		break;
//
//	case APP_MSG_IFTASK_I2CCOMM_DISK_READ_COMPLETE:
//		//xprintf("Res code %d\n", data);	// This is the same as fileOp.res
//		// the fileOp structure should have the results
//		xprintf("Read %d bytes from '%s'. Result code: %d \n",
//				fileOp.length, fileOp.fileName, fileOp.res);
//		if (fileOp.res) {
//			xprintf("Read error\n");
//		}
//		else {
//			// Success. Print the file here:
//			xprintf("Contents:\n%s\n", fileOp.buffer);
//		}
//
//		if_task_state = APP_IF_STATE_IDLE;
//		break;


	case APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN:
		// Not used at the moment
		break;

	default:
		// Here for events that are not expected in this state.
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
	if ((event >= APP_MSG_IFTASK_FIRST) && (event < APP_MSG_IFTASK_LAST)) {
		xprintf("IF Task unhandled event '%s' in '%s'\r\n", ifTaskEventString[event - APP_MSG_IFTASK_FIRST], ifTaskStateString[if_task_state]);
	}
	else {
		xprintf("IF Task unhandled event 0x%04x in '%s'\r\n", event, ifTaskStateString[if_task_state]);
	}
	XP_WHITE;

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * Sends an I2C message to the WW130.
 *
 * This asserts the /IP_INT pin, then prepares the message and readies the I2C module to send it,
 * then negates the /IP_INT pin.
 * The WW130 reacts to the rising edge of the /IP_INT pin and starts an I2C read.
 *
 * When the read is completed the I2C module generates a i2cs_cb_tx() callback
 * which is used to change the state machine and release the semaphore.
 */
static void sendI2CMessage(uint8_t * data, aiProcessor_msg_type_t messageType, uint16_t payloadLength) {

	interprocessor_interrupt_assert();
	i2ccomm_write_enable(data, messageType, payloadLength);	// Get the I2C dat ready to transmit.
	interprocessor_interrupt_negate();	// WW130 responds on the rising edge. It starts the I2Cread process
}

/********************************** FreeRTOS Task  *************************************/

/**
 * FreeRTOS task responsible for handling interface with WW130
 */
static void vIfTask(void *pvParameters) {
	APP_MSG_T       rxMessage;
	APP_MSG_DEST_T  txMessage;
	QueueHandle_t   targetQueue;
	APP_MSG_T 		sendMsg;

	APP_IF_STATE_E old_state;
	const char * eventString;
	APP_MSG_EVENT_E event;
	uint32_t rxData;

	// One-off initialisation here...

    XP_CYAN;
    // Observing these messages confirms the initialisation sequence
    xprintf("Starting IFTask\n");
    XP_WHITE;

	interprocessor_interrupt_init();

	app_i2ccomm_init();
	dbg_printf(DBG_LESS_INFO, "I2C slave instance %d configured at address 0x%02x\n", iic_id, EVT_I2CS_0_SLV_ADDR);
	dbg_printf(DBG_LESS_INFO, "I2C buffers have %d bytes, payload is %d\n",
			WW130_MAX_WBUF_SIZE, WW130_MAX_PAYLOAD_SIZE);

	// TODO can we do something to detect whether there is a WW130 present, and
	// maybe stay in the UNINIT or ERROR state if not?
	if_task_state = APP_IF_STATE_IDLE;

	// Clear the saved event. This means there is to be no saved message.
	// The mechanism allows messages to be deferred till we return to IDLE. One of the handling routines might set it later.
	savedMessage.msg_event = APP_MSG_NONE;

	// Let's kick things off by sending ourselves a message - to send a message to the BLE processor
	sendMsg.msg_event = APP_MSG_IFTASK_AWAKE;
	sendMsg.msg_data = 0;
	sendMsg.msg_parameter = 0;

	// But wait a short time if it is a cold boot, so the BLE processor is initialised and ready
	if (woken == APP_WAKE_REASON_COLD) {
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	if (xQueueSend(xIfTaskQueue, (void *)&sendMsg, __QueueSendTicksToWait) != pdTRUE) {
		xprintf("sendMsg=0x%x fail\r\n", sendMsg.msg_event);
	}

	// The task loops forever here, waiting for messages to arrive in its input queue
	for (;;)  {
		if (xQueueReceive ( xIfTaskQueue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {

			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

			// convert event to a string
			if ((event >= APP_MSG_IFTASK_FIRST) && (event < APP_MSG_IFTASK_LAST)) {
				eventString = ifTaskEventString[event - APP_MSG_IFTASK_FIRST];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nIF Task ");
			XP_WHITE;
			xprintf("received event '%s' (0x%04x). Rx data = 0x%08x\r\n", eventString, event, rxData);

			old_state = if_task_state;

			// switch on state - and call individual event handling functions
			switch (if_task_state) {

			case APP_IF_STATE_UNINIT:
				txMessage = flagUnexpectedEvent(rxMessage);
				break;

			case APP_IF_STATE_IDLE:
				txMessage = handleEventForIdle(rxMessage);
				break;

			case APP_IF_STATE_I2C_RX:
				// When a message arrives from the MKL62BA (exchanges initiated by MKL62BA)
				txMessage = handleEventForStateI2CRx(rxMessage);
				break;

			case APP_IF_STATE_I2C_TX:
				// When I2C interface is transmitting (exchanges initiated by MKL62BA)
				txMessage = handleEventForStateI2CTx(rxMessage);
				break;

			case APP_IF_STATE_I2C_SLAVE_TX:
				// When I2C interface is transmitting (exchanges initiated by HX6538)
				txMessage = handleEventForStateI2CSlaveTx(rxMessage);
				break;

			case APP_IF_STATE_I2C_SLAVE_RX:
				// When a message arrives from the MKL62BA (exchanges initiated by HX6538)
				txMessage = handleEventForStateI2CSlaveRx(rxMessage);
				break;

			case APP_IF_STATE_PA0:
				txMessage = handleEventForStatePA0(rxMessage);
				break;

			case APP_IF_STATE_DISK_OP:
				txMessage = handleEventForStateDiskOp(rxMessage);
				break;

			default:
				// should not happen
				txMessage = flagUnexpectedEvent(rxMessage);
				break;
			}

			if (old_state != if_task_state) {
				// state has changed
				XP_LT_CYAN;
				xprintf("IF Task state changed ");
				XP_WHITE;
				xprintf("from '%s' (%d) to '%s' (%d)\r\n",
						ifTaskStateString[old_state], old_state,
						ifTaskStateString[if_task_state], if_task_state);
			}

			// The processing functions might want us to execute a deferred message when we return to IDLE
			if ((if_task_state == APP_IF_STATE_IDLE) && (savedMessage.msg_event != APP_MSG_NONE)) {
				event = savedMessage.msg_event;
				XP_BROWN;
				xprintf("Issuing deferred event 0x%04x", event);
				XP_WHITE;
				// convert event to a string
				if ((event >= APP_MSG_IFTASK_FIRST) && (event < APP_MSG_IFTASK_LAST)) {
					xprintf(" '%s'\n", ifTaskEventString[event - APP_MSG_IFTASK_FIRST]);
				}
				else {
					xprintf("\n");
				}
				sendMsg = savedMessage;
				if(xQueueSend( xIfTaskQueue , (void *) &sendMsg , __QueueSendTicksToWait) != pdTRUE) {
					xprintf("IFTask sending event 0x%x failed\r\n", sendMsg.msg_event);
				}
				else {
					xprintf("IFTask sending event 0x%04x. Tx data = 0x%08x\r\n", sendMsg.msg_event, sendMsg.msg_data);
				}
				// now clear the saved event
				savedMessage.msg_event = APP_MSG_NONE;
			}

			// The processing functions might want us to send a message to another task
			if (txMessage.destination != NULL) {
				sendMsg = txMessage.message;
				targetQueue = txMessage.destination;

				if(xQueueSend( targetQueue , (void *) &sendMsg , __QueueSendTicksToWait) != pdTRUE) {
					xprintf("IFTask sending event 0x%x failed\r\n", sendMsg.msg_event);
				}
				else {
					xprintf("IFTask sending event 0x%04x. Tx data = 0x%08x\r\n", sendMsg.msg_event, sendMsg.msg_data);
				}
			}
		}
	}	// for(;;)
}

/*********************************** Interprocessor Interrupt Functions ************************************************/

#ifdef WW500
/**
 * Interrupt callback for interprocessor interrupt pin (interrupt from MKL63BA).
 *
 * It sends an event to the ifTask queue
 *
 * On the WW500, the interrupt pin is PB11.
 * PB11 is connected to the SW2 (FTDI) switch on the WWIF100 breakout board. (P18 on the MKL62BA).
 * (on the WW500.A01 this is a wire link).
 * You can press the SW2 button for a short time. If you press it for a long time, the MKL62BA will enter DFU mode.
 */
static void interprocessor_interrupt_cb(uint8_t group, uint8_t aIndex) {
    uint8_t value;
    APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

    hx_drv_gpio_get_in_value(AON_GPIO0, &value);

    XP_YELLOW
    dbg_printf(DBG_LESS_INFO, "\nMKL62BA interrupt Rx.");
    XP_WHITE;

    send_msg.msg_data = value;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN;
    dbg_printf(DBG_LESS_INFO, "Send to ifTask 0x%x\r\n", send_msg.msg_event);


	xQueueSendFromISR( xIfTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
	if( xHigherPriorityTaskWoken )  {
    	taskYIELD();
    }

	hx_drv_gpio_clr_int_status(AON_GPIO0);
}

/**
 * Configure PB11 as GPIO2 to be used as the inter-processor interrupt signal
 * This is the signal called /IP_INT.
 *
 * PB11 can either be an output to the MKL62BA - driven as 0 - or an input.
 *
 * When PB11 is an input then it can be enabled as an interrupt. This allows the MKL62BA to interrupt this chip
 * by driving PB11 low. That is - the same PB11 signal can be used by either side to interrupt the other.
 *
 * This registers a callback to interprocessor_interrupt_cb() which in turn sends an event to the comm_task loop,
 * which in turn sends a message to main_task
 *
 * TODO move to pinmux_init()?
 */
static void interprocessor_interrupt_init(void) {
    SCU_PAD_PULL_LIST_T pad_pull_cfg;
	uint8_t gpio_value;

	// Initialise PB11 as an input. Expect a pull-up to take it high.
	// This device can then set PB11 to an output at logic 0 to change the state of the pin.
    hx_drv_gpio_set_input(GPIO2);
    hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_GPIO2, 1);

	// Set PB11 PULL_UP
    hx_drv_scu_get_all_pull_cfg(&pad_pull_cfg);
	pad_pull_cfg.pb11.pull_en = SCU_PAD_PULL_EN;
	pad_pull_cfg.pb11.pull_sel = SCU_PAD_PULL_UP;
    hx_drv_scu_set_all_pull_cfg(&pad_pull_cfg);


	// The next commands prepare PB11 to be an interrupt input
	hx_drv_gpio_clr_int_status(GPIO2);
	hx_drv_gpio_cb_register(GPIO2, interprocessor_interrupt_cb);	// define ISR
	hx_drv_gpio_set_int_type(GPIO2, GPIO_IRQ_TRIG_TYPE_EDGE_FALLING);	// only when PB11 goes low
	//hx_drv_gpio_set_int_type(GPIO2, GPIO_IRQ_TRIG_TYPE_EDGE_BOTH);	// When PB11 goes low, then when it goes high
	hx_drv_gpio_set_int_enable(GPIO2, 1);	// 1 means enable interrupt

	hx_drv_gpio_get_in_value(GPIO2, &gpio_value);
	xprintf("Initialised PB11 (GPIO2) as input. Read %d\n", gpio_value);
}

/**
 * Switches /IP_INT from an interrupt input to an active low output.
 *
 * /IP_INT is PB11 on the HX6538, allocated to GPIO2
 *
 * This will interrupt the MKL62BA. The pin needs to be taken high again
 * by interprocessor_interrupt_negate() after a suitable delay.
 */
static void interprocessor_interrupt_assert(void) {

	// disable the interrupt, so we don't interrupt ourself
	hx_drv_gpio_set_int_enable(GPIO2, 0);	// 0 means disable interrupt

	// Sets PA0 as an output and drive low, then delay, then high, then set as an input
    hx_drv_gpio_set_output(GPIO2, GPIO_OUT_LOW);

#if 0
    // for testing:
	uint8_t pinValue;
	hx_drv_gpio_get_in_value(GPIO2, &pinValue);
	XP_LT_GREEN;
	xprintf("Set PB11 as an output, driven to 0 (GPIO2). Read back as %d\n", pinValue);
	XP_WHITE;
#else
	XP_LT_GREEN;
	xprintf("Assert inter-processor interrupt.\n");
	XP_WHITE;
#endif
}

/**
 * Switches /IP_INT from an output to an interrupt input.
 *
 * This is the second part of a process to interrupt the MKL62BA.
 * In the first part, interprocessor_interrupt_assert() took the pin low.
 */
static void interprocessor_interrupt_negate(void) {

	hx_drv_gpio_set_out_value(GPIO2, GPIO_OUT_HIGH);
#if 0
	// This for testing:
	uint8_t pinValue;
	hx_drv_gpio_get_in_value(GPIO2, &pinValue);

	XP_LT_GREEN;
	xprintf("Set PB11 as an output, drive to 1 (GPIO2). Read back as %d\n", pinValue);
	XP_WHITE;
#else
	XP_LT_GREEN;
	xprintf("Negate inter-processor interrupt.\n");
	XP_WHITE;
#endif

	// Now set PB11 as an input and prepare it to respond to interrupts from the MKL62BA.
	hx_drv_gpio_set_input(GPIO2);

	// The next commands prepare PB11 to be an interrupt input
	hx_drv_gpio_clr_int_status(GPIO2);
	hx_drv_gpio_set_int_enable(GPIO2, 1);	// 1 means enable interrupt
}

#else
/**
 * Interrupt callback for interprocessor interrupt pin (interrupt from MKL63BA).
 *
 * It sends an event to the ifTask queue
 *
 * On the WW130/Seeed Vision AI V2 combination, the interrupt pin is PA0.
 * PA0 is connected to the SW2 (FTDI) switch on the WWIF100 breakout board.
 * You can press the SW2 button for a short time. If you press it for a long time, the MKL62BA will enter DFU mode.
 *
 * On the WW500, the interrupt pin is PB11.
 * PB11 is connected to the SW2 (FTDI) switch on the WWIF100 breakout board. (P18 on the MKL62BA).
 * (on the WW500.A00 this is a wire link).
 * You can press the SW2 button for a short time. If you press it for a long time, the MKL62BA will enter DFU mode.
 */
static void interprocessor_interrupt_cb(uint8_t group, uint8_t aIndex) {
    uint8_t value;
    APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

    hx_drv_gpio_get_in_value(AON_GPIO0, &value);

    XP_YELLOW
    dbg_printf(DBG_LESS_INFO, "\nMKL62BA interrupt Rx. ");
    XP_WHITE;

    send_msg.msg_data = value;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_PA0_INT_IN;
    dbg_printf(DBG_LESS_INFO, "Send to ifTask 0x%x\r\n", send_msg.msg_event);


	xQueueSendFromISR( xIfTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
	if( xHigherPriorityTaskWoken )  {
    	taskYIELD();
    }

	hx_drv_gpio_clr_int_status(AON_GPIO0);
}

/**
 * Initialises PA0 pin to cause an interrupt.
 *
 * This registers a callback to interprocessor_interrupt_cb() which in turn sends an event to the comm_task loop,
 * which in turn sends a message to main_task
 *
 * This version is based on earlier WW130 work.
 *
 * TODO move to pinmux_init()?
 *
 */
static void interprocessor_interrupt_init(void) {
	uint8_t gpio_value;

	// PA0 can either be an output to the WW130 - driven as 0 - or an input.
	// When PA0 is an input then it can be enabled as an interrupt. This allows the WW130 to interrupt this board
	// by driving PA0 low. That is - the same PA0 signal can be used by either side to interrupt the other.

	// Initialise PA0 as an input. Expect a pull-up to take it high.
	// This device can then set PA0 to an output at logic 0 to change the state of the pin.
    hx_drv_gpio_set_input(AON_GPIO0);
    // Looks like we must use SCU_PA0_PINMUX_AON_GPIO0_2 (I/O) rather than SCU_PA0_PINMUX_AON_GPIO0_0 Input only).
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
	hx_drv_gpio_cb_register(AON_GPIO0, interprocessor_interrupt_cb);	// define ISR
	hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_FALLING);	// only when PA0 goes low
	//hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_BOTH);	// When PA0 goes low, then when it goes high
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt

	xprintf("Initialised PA0 (AON_GPIO0) as input. Read %d\n", gpio_value);
}

/**
 * Switches PA0 from an interrupt input to an active low output.
 *
 * This will interrupt the MKL62BA. The pin needs to be taken high again
 * by interprocessor_interrupt_negate() after a suitable delay.
 */
static void interprocessor_interrupt_assert(void) {
	uint8_t pinValue;

	// disable the interrupt, so we don't interrupt ourself
	hx_drv_gpio_set_int_enable(AON_GPIO0, 0);	// 0 means disable interrupt

	// Sets PA0 as an output and drive low, then delay, then high, then set as an input
    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);

    // for testing:
	hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
	XP_LT_GREEN;
	xprintf("Set PA0 as an output, driven to 0 (AON_GPIO0). Read back as %d\n", pinValue);
	XP_WHITE;
}

/**
 * Switches PA0 from an output to an interrupt input.
 *
 * This is the second part of a process to interrupt the MKL62BA.
 * In the first part, interprocessor_interrupt_assert() took the pin low.
 */
static void interprocessor_interrupt_negate(void) {
	uint8_t pinValue;

	hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);

	// This for testing:
	hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);

	XP_LT_GREEN;
	xprintf("Set PA0 as an output, drive to 1 (AON_GPIO0). Read back as %d\n", pinValue);
	XP_WHITE;

	// Now set PA0 as an input and prepare it to respond to interrupts from the WW130.
	hx_drv_gpio_set_input(AON_GPIO0);

	// The next commands prepare PA0 to be an interrupt input
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt
}
#endif	// ww500


#ifdef TEST_INT_PULSE

/**
 * FreeRTOS timer to release interprocessor interrupt pin
 *
 * We could just set the pin high at this point, but by sending an event to the queue
 * we can keep a more consistent track of events.
 */
static void onInterruptTimerExpired(xTimerHandle pxTimer) {
    APP_MSG_T send_msg;

    send_msg.msg_data = 0;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_PA0_TIMER;

	if(xQueueSend( xIfTaskQueue , (void *) &send_msg , __QueueSendTicksToWait) == pdTRUE) {
	    dbg_printf(DBG_LESS_INFO, "Sent to iftask 0x%x\r\n", send_msg.msg_event);
	}
	else {
		dbg_printf(DBG_LESS_INFO, "Sent to iftask 0x%x fail\r\n", send_msg.msg_event);
	}
}

#endif //  TEST_INT_PULSE


/*********************************** Other Local Function Definitions ************************************************/

/**
 * Initialises our I2C slave module
 *
 * Sets up our slave address and callback functions.
 *
 */
static uint16_t app_i2ccomm_init(void) {
    iic_id = USE_DW_IIC_SLV_0;
    int16_t ret;

    clear_read_buf_header();

    ret = hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg);
    if (ret) {
    	xprintf("DEBUG: hx_lib_i2ccomm_init() returns %d\n", ret);
    	return ret;
    }

    ret = hx_lib_i2ccomm_start(iic_id, (unsigned char *)gRead_buf, WW130_MAX_RBUF_SIZE);

    if (ret) {
    	xprintf("DEBUG: hx_lib_i2ccomm_start() returns %d\n", ret);
    	return ret;
    }

    // I think we need to enable reads here:
    ret = hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, WW130_MAX_RBUF_SIZE);
    if (ret) {
    	xprintf("DEBUG: hx_lib_i2ccomm_enable_read() returns %d\n", ret);
    }
	return ret;
}

/**
 * Clears I2C header bytes.
 * (sets 4 header bytes to 0xff) - why? and why not 0?
 *
 * Called in preparation for receiving a command from the I2C master
 */
static void clear_read_buf_header(void) {
    memset((void *)gRead_buf, 0xff, 4);
    hx_CleanDCache_by_Addr((void *) gRead_buf, I2CCOMM_MAX_RBUF_SIZE);
}

/**
 * Missing Master timer has expired.
 *
 *
 */
static void missingMasterExpired(xTimerHandle pxTimer) {
    APP_MSG_T send_msg;

    send_msg.msg_data = 0;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_MM_TIMER;

	if(xQueueSend( xIfTaskQueue , (void *) &send_msg , __QueueSendTicksToWait) == pdTRUE) {
	    dbg_printf(DBG_LESS_INFO, "Sent to iftask 0x%x\r\n", send_msg.msg_event);
	}
	else {
		dbg_printf(DBG_LESS_INFO, "Sent to iftask 0x%x fail\r\n", send_msg.msg_event);
	}
}

/*************************************** Exported Function Definitions *****************************/

/**
 * Called by app_main() to create this task and anything that it needs.
 *
 * The app_main() code will call vTaskStartScheduler() to begin FreeRTOS scheduler
 */
TaskHandle_t ifTask_createTask(int8_t priority, APP_WAKE_REASON_E wakeReason) {

    woken = wakeReason;

	if (priority < 0){
		priority = 0;
	}
//
//    aon_gpio0_interrupt_init();
//
//    app_i2ccomm_init();
//	dbg_printf(DBG_LESS_INFO, "I2C slave instance %d configured at address 0x%02x\n", iic_id, EVT_I2CS_0_SLV_ADDR);

	xIfTaskQueue = xQueueCreate( IFTASK_QUEUE_LEN, sizeof(APP_MSG_T) );

	if(xIfTaskQueue == 0) {
        xprintf("xIfTaskQueue creation failed!.\r\n");
		configASSERT(0);	// TODO add debug messages?
	}

#ifdef TEST_INT_PULSE
	// Timer for pulsing interprocessor interrupt pin to MKL62BA - test only
    // Create a timer that turns off the PA0 interrupt pulse
    timerInterruptPulse = xTimerCreate(
    		"timerInterprocessorInt", 					/* name */
			pdMS_TO_TICKS(INTERRUPTPULSEWIDTH), 	/* period/time */
			pdFALSE, 						/* NO auto reload */
			(void*)0, 						/* timer ID */
			onInterruptTimerExpired); 			/* callback */

    if (timerInterruptPulse == NULL) {
    	dbg_printf(DBG_LESS_INFO, "Failed to create timerInterruptPulse");
		configASSERT(0);	// TODO add debug messages?
    }
#endif	// TEST_INT_PULSE

    // Create a timer that deals with the master failing to read I2C data
    timerHndlMissingMaster = xTimerCreate(
    		"timerMM", 					/* name */
			pdMS_TO_TICKS(MISSINGMASTERTIME), 	/* period/time */
			pdFALSE, 						/* NO auto reload */
			(void*)1, 						/* timer ID */
			missingMasterExpired); 			/* callback */

    if (timerHndlMissingMaster == NULL) {
    	dbg_printf(DBG_LESS_INFO, "Failed to create timerHndlMissingMaster");
		configASSERT(0);	// TODO add debug messages?
    }

	// Semaphore to ensure only one CLI response goes to the WW130 at once
	xI2CTxSemaphore = xSemaphoreCreateBinary();
	if(xI2CTxSemaphore == NULL) {
		xprintf("Failed to create xI2CTxSemaphore\n");
		configASSERT(0);	// TODO add debug messages?
	}

	// Semaphore to flag that the final message has been sent and we can enter DPD
	xIfCanSleepSemaphore = xSemaphoreCreateBinary();

	if(xIfCanSleepSemaphore == NULL) {
		xprintf("Failed to create xIfCanSleepSemaphore\n");
		configASSERT(0);	// TODO add debug messages?
	}

	// Now must release the I2C semaphore
	xSemaphoreGive(xI2CTxSemaphore);

	if (xTaskCreate(vIfTask, (const char *)"IFTask",
			configMINIMAL_STACK_SIZE * 3,
			NULL, priority,
			&ifTask_task_id) != pdPASS)  {
        xprintf("vIfTask creation failed!.\r\n");
		configASSERT(0);	// TODO add debug messages?
	}

	return ifTask_task_id;
}


/**
 * Returns the internal state as a number
 */
uint16_t ifTask_getState(void) {
	return if_task_state;
}

/**
 * Returns the internal state as a string
 */
const char * ifTask_getStateString(void) {
	return * &ifTaskStateString[if_task_state];
}

