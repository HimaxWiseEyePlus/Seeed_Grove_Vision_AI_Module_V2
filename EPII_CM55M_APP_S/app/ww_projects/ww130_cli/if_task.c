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
#include "if_task.h"
#include "ww130_cli.h"
#include "i2c_comm.h"

#include "WE2_debug.h"

#include "hx_drv_gpio.h"
#include "hx_drv_scu.h"

#include "crc16_ccitt.h"

/*************************************** Definitions *******************************************/

#define IFTASK_QUEUE_LEN   		10
#define EVT_I2CS_0_SLV_ADDR         0x62

// Time in ms that the PA0 is low
#define PA0PULSEWIDTH	120

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

/*************************************** Local Function Declarations *****************************/

static void vIfTask(void *pvParameters);

// These are separate event handlers, one for each of the possible state machine state
static APP_MSG_DEST_T  handleEventForIdle(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStateI2C(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForStatePA0(APP_MSG_T rxMessage);
static APP_MSG_DEST_T  handleEventForError(APP_MSG_T rxMessage);

// This is to process an unexpected event
static APP_MSG_DEST_T  flagUnexpectedEvent(APP_MSG_T rxMessage);

static void app_i2ccomm_init();
static void aon_gpio0_cb(uint8_t group, uint8_t aIndex);
static void aon_gpio0_interrupt_init(void);

static void aon_gpio0_drive_low(void);
static void aon_gpio0_drive_high(void);

static void i2cs_cb_tx(void *param);
static void i2cs_cb_rx(void *param);
static void i2cs_cb_err(void *param);

static void evt_i2ccomm_rx_cb(void);
static void clear_read_buf_header(void);

static void pa0InterruptExpired(xTimerHandle pxTimer);


// I2C slave address and callbacks - used for initialisation only
I2CCOMM_CFG_T gI2CCOMM_cfg = {
        EVT_I2CS_0_SLV_ADDR,
        i2cs_cb_tx,
        i2cs_cb_rx,
        i2cs_cb_err
};

/*************************************** Local variables *******************************************/

// which I2C slave instance to use
static USE_DW_IIC_SLV_E iic_id;

// This is the handle of the task
TaskHandle_t ifTask_task_id;

QueueHandle_t     xIfTaskQueue;
extern QueueHandle_t     xCliTaskQueue;

volatile APP_IF_STATE_E if_task_state = APP_IF_STATE_UNINIT;

// Timer for pulsing PA0 to WW130
xTimerHandle timerHndlPa0;

//I2C incoming and outgoing buffers
uint8_t gWrite_buf[I2CCOMM_MAX_WBUF_SIZE];
uint8_t gRead_buf[I2CCOMM_MAX_RBUF_SIZE];

// Strings for each of these states. Values must match APP_TASK1_STATE_E in task1.h
const char * ifTaskStateString[APP_IF_STATE_ERROR + 1] = {
		"Uninitialised",
		"Idle",
		"I2C State",
		"PA0 State",
		"Error"
};

// Strings for expected messages. Values must match Messages directed to IF Task in app_msg.h
// Experiment: If I define xxx_FIRST and xx_LAST a I have done, then this should work:
const char* ifTaskEventString[APP_MSG_IFTASK_LAST - APP_MSG_IFTASK_FIRST] = {
		"I2C Rx",
		"I2C Tx",
		"I2C Error",
		"PA0 Interrupt",
		"PA0 Timer",
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

/*********************************** I2C Local Function Definitions ************************************************/


/**
 * I2C slave callback - called when the Master has read our I2C data.
 *
 * This in in the ISR context. We pass an event to the comm_task loop which then
 * calls evt_i2ccomm_tx_cb() in the task's main thread.
 */
void i2cs_cb_tx(void *param) {
	//HX_DRV_DEV_IIC *iic_obj = param;
	//HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

	//send_msg.msg_data = iic_info_ptr->slv_addr;
	send_msg.msg_data = 0;
	send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_TX;

	dbg_printf(DBG_LESS_INFO, "I2C TX ISR. Send to ifTask 0x%x\r\n", send_msg.msg_event);

	xQueueSendFromISR( xIfTaskQueue, &send_msg, &xHigherPriorityTaskWoken );
	if( xHigherPriorityTaskWoken )  {
		taskYIELD();
	}
}

/**
 * I2C slave callback - when we receive data from the master.
 * Called in response to hx_lib_i2ccomm_enable_write()
 *
 * This in in the ISR context. Wee pass an event to the comm_task loop which then
 * calls evt_i2ccomm_0_rx_cb() in the task's main thread.
 */
void i2cs_cb_rx(void *param) {
    //HX_DRV_DEV_IIC *iic_obj = param;
    //HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

        send_msg.msg_data = 0;
    	send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_RX;

        dbg_printf(DBG_LESS_INFO, "I2C RX ISR. Send to ifTask 0x%x\r\n", send_msg.msg_event);

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
 */
void i2cs_cb_err(void *param) {
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

    send_msg.msg_data = iic_info_ptr->err_state;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_ERR;

    dbg_printf(DBG_LESS_INFO, "I2C Error ISR %d Send to ifTask 0x%x\r\n", iic_info_ptr->err_state, send_msg.msg_event);

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
uint8_t evt_i2ccomm_tx_cb(void) {
	XP_YELLOW;
	dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("I2C transmission complete.\n");
    XP_WHITE;

    // Prepare for the next incoming message
    clear_read_buf_header();
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, I2CCOMM_MAX_RBUF_SIZE);
    return 0;
}

/**
 * Called when the I2C master sends us a message
 *
 * Called from within the ifTask loop in response to the interrupt callback in i2cs_cb_rx()
 */
static void evt_i2ccomm_rx_cb(void) {
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
	XP_WHITE;

	crcOK = crc16_ccitt_validate(gRead_buf, I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE );

	// Let's print the message
	printf_x_printBuffer(gRead_buf, (I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE ));

	// Now overwrite the checksum with 0 so the payload becomes a string
	gRead_buf[I2CCOMM_HEADER_SIZE + length] = '\0';

	if  ((feature != WW130_CMD_FEATURE) || (cmd >= AI_PROCESSOR_MSG_END)) {
		xprintf("Received feature=%d cmd=%d length=%d \n", feature, cmd, length);
		xprintf("Command is not recognised\n");
		return;
	}

	xprintf("Received feature=%d cmd=%d (%s) length=%d '%s' ",
			feature, cmd, cmdString[cmd], length, (char * ) payload);

	// Check the CRC
	if(crcOK) {
		xprintf("CRC OK\n");
	}
	else {
		xprintf("CRC failed\n");
		return;
	}

	// Default message, which hopefully will be over-written
	//snprintf((char *) returnMessage, WW130_MAX_PAYLOAD_SIZE, "Unrecognised command");

	// Now let's see if we can make sense of the message
	switch (cmd) {
	case AI_PROCESSOR_MSG_TX_STRING:
		// I2C master has sent a string. We assume it is a command to execute
		xprintf("WW130 command received: '%s'\n", (char * ) payload);
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


    // Prepare for the next incoming message
	clear_read_buf_header();
	hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, I2CCOMM_MAX_RBUF_SIZE);
}

/**
 * Slave I2C transmission to master has an error.
 *
 * Called from within the ifTask loop in response to the interrupt callback in i2cs_cb_err()
 *
 * This readies us for a new command from the master
 */

static void evt_i2ccomm_err_cb(void) {
	XP_LT_RED;
	dbg_evt_iics_cmd("\n");
	dbg_evt_iics_cmd("I2C error\n");
	XP_WHITE;

    // Prepare for the next incoming message
    clear_read_buf_header();
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, I2CCOMM_MAX_RBUF_SIZE);
}


/*************************************** Local Function Definitions *****************************/

/**
 * Implements state machine when in APP_TASK1_STATE_INIT
 *
 * Let's say that in APP_TASK1_STATE_INIT we expect only APP_MSG_TASK1_MSG0 and use it to switch to APP_TASK1_STATE_2
 * And that any other event moves us to APP_TASK1_STATE_ERROR
 */
static APP_MSG_DEST_T handleEventForIdle(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	// Do this to stop compiler warnings for unused variables:
	(void) event;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_IFTASK_I2CCOMM_RX:
		// Here when I2C message arrived
		if_task_state = APP_IF_STATE_I2C;
		evt_i2ccomm_rx_cb();
		break;

	case APP_MSG_IFTASK_I2CCOMM_PA0_INT:
		xprintf("Sorry, I haven't got round to dealing with this yet!\n");
	break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		if_task_state = APP_IF_STATE_ERROR;
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

static APP_MSG_DEST_T  handleEventForStateI2C(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	// Do this to stop compiler warnings for unused variables:
	(void) event;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

	switch (event) {
	case APP_MSG_IFTASK_I2CCOMM_RX:
	case APP_MSG_IFTASK_I2CCOMM_TX:
	case APP_MSG_IFTASK_I2CCOMM_ERR:
		xprintf("Sorry, I haven't got round to dealing with this yet!\n");
		break;

	case APP_MSG_IFTASK_I2CCOMM_PA0_INT:
		xprintf("Sorry, I haven't got round to dealing with this yet!\n");
	break;

	default:
		// Here for events that are not expected in this state.
		flagUnexpectedEvent(rxMessage);
		if_task_state = APP_IF_STATE_ERROR;
		break;
	}

	// If non-null then our task sends another message to another task
	return sendMsg;
}

static APP_MSG_DEST_T  handleEventForStatePA0(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;

	// Do this to stop compiler warnings for unused variables:
	(void) event;

	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;

//	switch (event) {
//	case APP_MSG_IFTASK_MSG0:
//		//
//		if_task_state = APP_IF_STATE_I2C;
//		break;
//
//	default:
//		// Here for events that are not expected in this state.
//		flagUnexpectedEvent(rxMessage);
//		if_task_state = APP_IF_STATE_ERROR;
//		break;
//	}

	// If non-null then our task sends another message to another task
	return sendMsg;

}

static APP_MSG_DEST_T  handleEventForError(APP_MSG_T rxMessage) {
	APP_MSG_EVENT_E event;
	//uint32_t data;
	APP_MSG_DEST_T sendMsg;
	sendMsg.destination = NULL;


	// Do this to stop compiler warnings for unused variables:
	(void) event;


	event = rxMessage.msg_event;
	//data = rxMessage.msg_data;
//
//	switch (event) {
//	case APP_MSG_IFTASK_MSG0:
//		//
//		if_task_state = APP_IF_STATE_I2C;
//		break;
//
//	default:
//		// Here for events that are not expected in this state.
//		flagUnexpectedEvent(rxMessage);
//		if_task_state = APP_IF_STATE_ERROR;
//		break;
//	}

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
		xprintf("UNHANDLED event '%s' in '%s'\r\n", ifTaskEventString[event - APP_MSG_IFTASK_FIRST], ifTaskStateString[if_task_state]);
	}
	else {
		xprintf("UNHANDLED event 0x%04x in '%s'\r\n", event, ifTaskStateString[if_task_state]);
	}
	XP_WHITE;

	// If non-null then our task sends another message to another task
	return sendMsg;
}

/**
 * FreeRTOS task responsible for handling interface with WW130
 */
static void vIfTask(void *pvParameters) {
    APP_MSG_T       rxMessage;
    APP_MSG_DEST_T  txMessage;
	QueueHandle_t   targetQueue;
    APP_MSG_T 		send_msg;

    APP_IF_STATE_E old_state;
	const char * eventString;
	APP_MSG_EVENT_E event;
	uint32_t rxData;

	// One-off initialisation here...

	if_task_state = APP_IF_STATE_IDLE;

	// The task loops forever here, waiting for messages to arrive in its input queue
	// The task loops forever here, waiting for messages to arrive in its input queue
	for (;;)  {
		if (xQueueReceive ( xIfTaskQueue , &(rxMessage) , __QueueRecvTicksToWait ) == pdTRUE ) {
			// convert event to a string
			event = rxMessage.msg_event;
			rxData =rxMessage.msg_data;

			if ((event >= APP_MSG_IFTASK_FIRST) && (event < APP_MSG_IFTASK_LAST)) {
				eventString = ifTaskEventString[event - APP_MSG_IFTASK_FIRST];
			}
			else {
				eventString = "Unexpected";
			}

			XP_LT_CYAN
			xprintf("\nIF Task");
			XP_WHITE;
			xprintf(" received event '%s' (0x%04x). Value = 0x%08x\r\n", eventString, event, rxData);\

			old_state = if_task_state;

    		// switch on state - and call individual event handling functions
    		switch (if_task_state) {

    		case APP_IF_STATE_UNINIT:
    			txMessage = flagUnexpectedEvent(rxMessage);
    			break;

    		case APP_IF_STATE_IDLE:
    			txMessage = handleEventForIdle(rxMessage);
    			break;

    		case APP_IF_STATE_I2C:
    			txMessage = handleEventForStateI2C(rxMessage);
    			break;

    		case APP_IF_STATE_PA0:
    			txMessage = handleEventForStatePA0(rxMessage);
    			break;

    		case APP_IF_STATE_ERROR:
    			txMessage = handleEventForError(rxMessage);
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

/*********************************** GPIO Pin Local Function Definitions ************************************************/

/**
 * Interrupt callback for PA0 pin.
 *
 * It sends an event to the ifTask queue
 *
 * On the WW130/Seeed Vision AI V2 combination, PA0 is connected to the SW2 (FTDI) switch.
 * You can press the SW2 button for a short time. If you press it for a long time, the WW130 will enter DFU mode.
 */
static void aon_gpio0_cb(uint8_t group, uint8_t aIndex) {
    uint8_t value;
    APP_MSG_T send_msg;
	BaseType_t xHigherPriorityTaskWoken;

    hx_drv_gpio_get_in_value(AON_GPIO0, &value);

    XP_YELLOW
    dbg_printf(DBG_LESS_INFO, "\nPA0 interrupt. ");
    XP_WHITE;

    send_msg.msg_data = value;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_PA0_INT;
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
 * This registers a callback to aon_gpio0_cb() which in turn sends an event to the comm_task loop,
 * which in turn sends a message to main_task
 *
 * This version is based on earlier WW130 work.
 *
 * TODO move to pinmux_init()?
 *
 */
static void aon_gpio0_interrupt_init(void) {
	uint8_t gpio_value;

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

	xprintf("Initialised PA0 (AON_GPIO0) as input. Read %d\n", gpio_value);
}

/**
 * Switches PA0 from an interrupt input to an active low output.
 *
 * This will interrupt the WW130. Thie pin needs to be taken high agin
 * by aon_gpio0_drive_high() after a suitable delay.
 */
static void aon_gpio0_drive_low(void) {
	uint8_t pinValue;

	// disable the interrupt, so we don't interrupt ourself
	hx_drv_gpio_set_int_enable(AON_GPIO0, 0);	// 0 means disable interrupt

	// Sets PA0 as an output and drive low, then delay, then high, then set as an input
    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);

    // for testing:
	hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
	XP_LT_RED;
	xprintf("Set PA0 as an output, driven to 0 (AON_GPIO0). Read back as %d\n", pinValue);
	XP_WHITE;
}

/**
 * Switches PA0 from an output to an interrupt input.
 *
 * This is the second part of a process to interrupt the WW130.
 * In the first part, aon_gpio0_drive_low() took the pin low.
 */
static void aon_gpio0_drive_high(void) {
	uint8_t pinValue;

	hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);

	// This for testing:
	hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);

	XP_LT_RED;
	xprintf("Set PA0 as an output, drive to 1 (AON_GPIO0). Read back as %d\n", pinValue);
	XP_WHITE;

	// Now set PA0 as an input and prepare it to respond to interrupts from the WW130.
	hx_drv_gpio_set_input(AON_GPIO0);

	// The next commands prepare PA0 to be an interrupt input
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt
}

/**
 * FreeRTOS timer to release PA0
 *
 * We could just set the pin high at this point, but by sending an event to the queue
 * we can keep a more consistent track of events.
 */
static void pa0InterruptExpired(xTimerHandle pxTimer) {
    APP_MSG_T send_msg;

    send_msg.msg_data = 0;
    send_msg.msg_event = APP_MSG_IFTASK_I2CCOMM_PA0_TIMER;

	if(xQueueSend( xIfTaskQueue , (void *) &send_msg , __QueueSendTicksToWait) == pdTRUE) {
	    dbg_printf(DBG_LESS_INFO, "Sent to comm task 0x%x\r\n", send_msg.msg_event);
	}
	else {
		dbg_printf(DBG_LESS_INFO, "send comm_send_msg=0x%x fail\r\n", send_msg.msg_event);
	}
}

/*********************************** Other Local Function Definitions ************************************************/

/**
 * Initialises our I2C slave module
 *
 * Sets up our slave address and callback functions.
 *
 */
static void app_i2ccomm_init(void) {
    iic_id = USE_DW_IIC_SLV_0;
    int16_t ret;

    ret = hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg);

    xprintf("DEBUG: hx_lib_i2ccomm_init() returns %d\n", ret);
    clear_read_buf_header();
    ret = hx_lib_i2ccomm_start(iic_id, (unsigned char *)gRead_buf, I2CCOMM_MAX_RBUF_SIZE);

    xprintf("DEBUG: hx_lib_i2ccomm_start() returns %d\n", ret);

    // I think we need to enable reads here:
    ret = hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) gRead_buf, I2CCOMM_MAX_RBUF_SIZE);
    xprintf("DEBUG: hx_lib_i2ccomm_enable_read() returns %d\n", ret);
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

/*************************************** Exported Function Definitions *****************************/

/**
 * Called by app_main() to create this task and anything that it needs.
 *
 * The app_main() code will call vTaskStartScheduler() to begin FreeRTOS scheduler
 */
void ifTask_createTask(void) {

    aon_gpio0_interrupt_init();

    app_i2ccomm_init();
	dbg_printf(DBG_LESS_INFO, "I2C slave instance %d configured at address 0x%02x\n", iic_id, EVT_I2CS_0_SLV_ADDR);

    // Initialise any inter-board communications and provide a callback for I2C messages
    //ww130_cmd_init();

	xIfTaskQueue = xQueueCreate( IFTASK_QUEUE_LEN, sizeof(APP_MSG_T) );

	if(xIfTaskQueue == 0) {
        xprintf("xIfTaskQueue creation failed!.\r\n");
		configASSERT(0);	// TODO add debug messages?
	}

    // Create a timer that turns off the PA0 interrupt pulse
    timerHndlPa0 = xTimerCreate(
    		"timerPa0", 					/* name */
			pdMS_TO_TICKS(PA0PULSEWIDTH), 	/* period/time */
			pdFALSE, 						/* NO auto reload */
			(void*)0, 						/* timer ID */
			pa0InterruptExpired); 			/* callback */

    if (timerHndlPa0 == NULL) {
    	dbg_printf(DBG_LESS_INFO, "Failed to create timerHndlPa0");
		configASSERT(0);	// TODO add debug messages?
    }
    else {
    	dbg_printf(DBG_LESS_INFO, "PA0 will pulse for %dms", PA0PULSEWIDTH);
    }

	if (xTaskCreate(vIfTask, (const char *)"IFTask",
			configMINIMAL_STACK_SIZE,
			NULL, if_task_PRIORITY,
			&ifTask_task_id) != pdPASS)  {
        xprintf("vIfTask creation failed!.\r\n");
		configASSERT(0);	// TODO add debug messages?
	}
}


/**
 * Returns the internal state as a number
 */
uint16_t ifTask_getTaskState(void) {
	return if_task_state;
}

/**
 * Returns the internal state as a string
 */
const char * ifTask_getTaskStateString(void) {
	return * &ifTaskStateString[if_task_state];
}

