/*
 * comm_task.c
 *
 *  Created on: 2022/02/22
 *      Author: 902447
 *
 * FreeRTOS task that handles GPIO interrupts and I2C slave comms
 *
 */

/*********************************** Includes ***********************************************/

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "WE2_device.h"
#include "WE2_core.h"
#include "WE2_debug.h"
#include "board.h"
#include "xprintf.h"
#include "printf_x.h"
#include "ctype.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif

#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
#ifdef TRUSTZONE_SEC
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
#ifdef NSC
#include "veneer_table.h"
#endif
/* Trustzone config. */

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif
#endif
#include "app_msg.h"
#include "app_state.h"
#include "xprintf.h"
#include "board.h"
#include "i2c_comm.h"
#include "evt_i2ccomm.h"
#include "app_msg.h"
#include "app_state.h"
#include "dp_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "ww130_test.h"
#include "ww130_cmd.h"

/*********************************** Definitions ***********************************************/
#define USECRC16CCITT
#ifdef USECRC16CCITT
// CRC code from nRF52832 work
#include "crc16_ccitt.h"
#endif	//USECRC16CCITT

//#define CRCTEST

#define DBG_EVT_IICS_CMD_LOG             (1)
#define DBG_EVT_IICS_CALLBACK_LOG        (0)

#define DATA_SFT_OFFSET_0           0
#define DATA_SFT_OFFSET_8           8
#define DATA_SFT_OFFSET_16          16
#define DATA_SFT_OFFSET_24          24

#define EVT_I2CS_0_SLV_ADDR         0x62
//#define EVT_I2CS_1_SLV_ADDR         0x64

// Time in ms that the PA0 is low
#define PA0PULSEWIDTH	1

/*********************************** Debug Function Definitions ***********************************************/

#if DBG_EVT_IICS_CMD_LOG
    #define dbg_evt_iics_cmd(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cmd(fmt, ...)
#endif

#if DBG_EVT_IICS_CALLBACK_LOG
    #define dbg_evt_iics_cb(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cb(fmt, ...)
#endif


/*********************************** Local variables ***********************************************/

// which I2C slave instance to use
static USE_DW_IIC_SLV_E iic_id;

// 192		// MUST BE (a multiple of 32 BLE device does not deal with > 255

unsigned char gWrite_buf[1][I2CCOMM_MAX_WBUF_SIZE];
unsigned char gRead_buf[1][I2CCOMM_MAX_RBUF_SIZE];

extern QueueHandle_t     xMainTaskQueue;
extern QueueHandle_t     xCommTaskQueue;
extern volatile APP_COMM_TASK_STATE_E g_commtask_state;

// Timer for pulsing PA0 to WW130
xTimerHandle timerHndlPa0;

/*********************************** Local Function Declarations **********************************/

//static void aon_gpio0_interupt_reg();
//static void aon_gpio1_interupt_reg();

static void app_i2ccomm_init();
static void aon_gpio0_cb(uint8_t group, uint8_t aIndex);
//static void aon_gpio1_cb(uint8_t group, uint8_t aIndex);
static void aon_gpio0_interupt_init(void);
//static void aon_gpio1_interupt_init(void);

static void aon_gpio0_drive_low(void);
static void aon_gpio0_drive_high(void);

void i2cs_cb_tx(void *param);
void i2cs_cb_rx(void *param);
void i2cs_cb_err(void *param);
uint8_t evt_i2ccomm_0_rx_cb(void);
void evt_i2cs_cmd_process_sysinfo(void);
static void i2ccomm_clear_read_buf_header(void);
#ifdef CRCTEST
// Test how the CRC generation and checking works
static void check_crc(void);
#endif	// CRCTEST

// Customer callback
typedef void (*i2ccomm_customer)(void*);

//static bool acutetech_utils_inISR(void);
static void pa0InterruptExpired(xTimerHandle pxTimer);

// Callback for our client, for certain I2C 'features' in the range 80-8f
i2ccomm_customer i2ccomm_cmd_customer_process = NULL;

// I2C slave address and callbacks - used for initialisation only
I2CCOMM_CFG_T gI2CCOMM_cfg[1] = {
    {
        EVT_I2CS_0_SLV_ADDR,
        i2cs_cb_tx,
        i2cs_cb_rx,
        i2cs_cb_err}
};

/*********************************** I2C Local Function Definitions ************************************************/

#ifdef CRCTEST
// Test how the CRC generation and checking works
// Th results printed here should be compared with those printed by the ble_app_crc app
static void check_crc(void) {
	int i;
	size_t datalen;
	uint16_t crc_hx;
	uint16_t crc_table;
	uint16_t crc_production;

	uint16_t crc_check_algorithm;
	uint16_t crc_check_table;
	uint16_t ret_val;

	/* Use the examples from
	 * http://srecord.sourceforge.net/crc16-ccitt.html */
	char *testdata[] = {
		/* Empty string */
		"",
		"A",
		"123456789",
		/* 256 x "A" */
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
		NULL
	};

	uint8_t temp_buffer[300];
// CHECKSUM
	for (i = 0; testdata[i]; i++) {
		dbg_printf(DBG_LESS_INFO, "Data: '%s'\n", testdata[i]);

		/* Calculate the CRCs */
		datalen = strlen(testdata[i]);
		//crc_hx = crc_calculate_algorithm(testdata[i], datalen);
        hx_lib_i2ccomm_generate_checksum((unsigned char *) testdata[i], datalen, &crc_hx);

		dbg_printf(DBG_LESS_INFO, "hx_lib CRC:       0x%04X\n", crc_hx);

		/* Add the algorithm CRC after the data, note the big endian
		 * byte order */

//		memcpy(temp_buffer, testdata[i], datalen);
//
//		temp_buffer[datalen] = crc_hx >> 8;
//		temp_buffer[datalen + 1] = crc_hx & 0xFF;
//
//
//	    retval = hx_lib_i2ccomm_validate_checksum((unsigned char *) temp_buffer);
//
//		//ret_val = crc16_ccitt_validate((uint8_t *) temp_buffer, datalen + 2);
//
//		dbg_printf(DBG_LESS_INFO, "Production CRC check %s\n", (ret_val == NRF_SUCCESS)? "OK":"Failed");
	}
	dbg_printf(DBG_LESS_INFO, "\n" );
}
#endif // CRCTEST

/**
 * I2C slave callback - called when the Master has read our I2C data.
 *
 * This in in the ISR context. We pass an event to the comm_task loop which then
 * calls evt_i2ccomm_0_tx_cb() in the task's main thread.
 */
void i2cs_cb_tx(void *param) {
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;

	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        //dbg_evt_iics_cb("%s (iic_id:0) \n", __FUNCTION__);

        comm_send_msg.msg_data = iic_info_ptr->slv_addr;
        comm_send_msg.msg_event = APP_MSG_COMMEVENT_I2CCOMM_TX;

        dbg_printf(DBG_LESS_INFO, "I2C TX ISR. Send to comm task 0x%x\r\n", comm_send_msg.msg_event);

        // DEBUG check the contents of the write buffer to see if it is intact
        //printf_x_printBuffer((uint8_t *) &gWrite_buf[iic_id], I2CCOMM_MAX_WBUF_SIZE);

    	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken )  {
        	taskYIELD();
        }
    }
    else {
        dbg_evt_iics_cb("%s(iic_id:1) txcb not support\n", __FUNCTION__);
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
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s (iic_id:0) \n", __FUNCTION__);

        comm_send_msg.msg_data = iic_info_ptr->slv_addr;
        comm_send_msg.msg_event = APP_MSG_COMMEVENT_I2CCOMM_RX;
        dbg_printf(DBG_LESS_INFO, "I2C RX ISR. Send to comm task 0x%x\r\n", comm_send_msg.msg_event);

        // DEBUG check the contents of the write buffer to see if it is intact
        //printf_x_printBuffer((uint8_t *) &gWrite_buf[iic_id], I2CCOMM_MAX_WBUF_SIZE);

        xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken )  {
        	taskYIELD();
        }
    }
    else {
        dbg_evt_iics_cb("%s(iic_id:1) rxcb not support\n", __FUNCTION__);
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

    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR) {
        dbg_evt_iics_cb("%s (iic_id:0) Error: %d \n", __FUNCTION__, iic_info_ptr->err_state);

        comm_send_msg.msg_data = iic_info_ptr->err_state;
        comm_send_msg.msg_event = APP_MSG_COMMEVENT_I2CCOMM_ERR;
        dbg_printf(DBG_LESS_INFO, "I2C Error ISR %d Send to comm task 0x%x\r\n", iic_info_ptr->err_state, comm_send_msg.msg_event);
    	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken )  {
        	taskYIELD();
        }
    }
    else {
        dbg_evt_iics_cb("%s(iic_id:1) errcb not support\n", __FUNCTION__);
    }
}

/**
 * Slave I2C transmission to master is complete.
 *
 * Called from within the comm_task loop in response to the interrupt callback in i2cs_cb_tx()
 *
 * This readies us for a new command from the master
 */
uint8_t evt_i2ccomm_0_tx_cb(void) {
	XP_YELLOW;
	dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("I2C transmission complete.\n");
    XP_WHITE;

    // Prepare for the next incoming message
    i2ccomm_clear_read_buf_header();
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
    return 0;
}

/**
 * Called when the I2C master sends us a message
 *
 * Called from within the comm_task loop in response to the interrupt callback in i2cs_cb_rx()
 */
uint8_t evt_i2ccomm_0_rx_cb(void) {
	//xprintf("0x%02x 0x%02x 0x%02x 0x%02x (rx.s)\n", gRead_buf[iic_id][0], gRead_buf[iic_id][1], gRead_buf[iic_id][2], gRead_buf[iic_id][3]);
	//hx_InvalidateDCache_by_Addr((uint32_t) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
	//xprintf("0x%02x 0x%02x 0x%02x 0x%02x (rx.e)\n", gRead_buf[iic_id][0], gRead_buf[iic_id][1], gRead_buf[iic_id][2], gRead_buf[iic_id][3]);

	uint8_t ret = 0;
	unsigned char feature = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
	XP_YELLOW;
	dbg_evt_iics_cmd("\n");
	dbg_evt_iics_cmd("Received I2C message. (Feature 0x%02x) \n", feature);
	XP_WHITE;

	switch (feature) {

	case I2CCOMM_FEATURE_SYS:
		// Here if the 1st byte of the I2C master message is 0
		// This is some obscure Himax functionality
		evt_i2cs_cmd_process_sysinfo();
		i2ccomm_clear_read_buf_header();
		break;

	case WW130_CMD_FEATURE:
		// Value 80 - this is "Wildlife.ai feature"
		// For WW130 communications, the WW130 should send a message with feature = I2CCOMM_FEATURE_CUSTOMER_MIN
		// Executes callback for our client (if registered)
		// The callback receives a pointer to the incoming I2C message
		if (i2ccomm_cmd_customer_process != NULL) {
			i2ccomm_cmd_customer_process(&gRead_buf[iic_id]);
		}
		break;

// I think all other values should have the default treatment, or the RX won't happen again?
//	case (I2CCOMM_FEATURE_CUSTOMER_MIN + 1) ... I2CCOMM_FEATURE_CUSTOMER_MAX:
//		// Values 81-8f
//		break;
//
//	case I2CCOMM_FEATURE_MAX:
//		ret = -1;
//		break;

	default:
		// There are plenty of other 'feature' values in evt_i2ccomm.h
	    // Prepare for the next incoming message
		i2ccomm_clear_read_buf_header();
		hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
		ret = -1;
		break;
	}

	return ret;
}

/**
 * Slave I2C transmission to master has an error.
 *
 * This readies us for a new command from the master
 */

uint8_t evt_i2ccomm_0_err_cb(void) {
	XP_LT_RED;
	dbg_evt_iics_cmd("\n");
	dbg_evt_iics_cmd("I2C error\n");
	XP_WHITE;

    // Prepare for the next incoming message
    i2ccomm_clear_read_buf_header();
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
    return 0;
}


/*********************************** GPIO Pin Local Function Definitions ************************************************/

/**
 * Interrupt callback for PA0 pin.
 *
 * It sends an event to the comm_task queue, which in turn sends a call to main_task
 *
 * On the WW130/Seeed Vision AI V2 combination, PA0 is connected to the SW2 (FTDI) switch.
 * You can press the SW2 button for a short time. If you ress it for a long time, the WW130 will enter DFU mode.
 *
 */
static void aon_gpio0_cb(uint8_t group, uint8_t aIndex) {
    uint8_t value;
    APP_MSG_T comm_send_msg;
	BaseType_t xHigherPriorityTaskWoken;
	/* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    hx_drv_gpio_get_in_value(AON_GPIO0, &value);

    XP_YELLOW
    //dbg_printf(DBG_LESS_INFO, "\naon_gpio0_cb (%d, %d, %d)\n", group, aIndex, value);
    dbg_printf(DBG_LESS_INFO, "\nPA0 interrupt. ");
    XP_WHITE;

    comm_send_msg.msg_data = value;
    comm_send_msg.msg_event = APP_MSG_COMMEVENT_AON_GPIO0_INT;
    dbg_printf(DBG_LESS_INFO, "Send to comm task 0x%x\r\n", comm_send_msg.msg_event);
	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
	hx_drv_gpio_clr_int_status(AON_GPIO0);

	if( xHigherPriorityTaskWoken )  {
    	taskYIELD();
    }
}

// Not used
///**
// * Interrupt callback for ? pin.
// *
// * It sends an event to the comm_task queue, which in turn sends a call to main_task
// */
//void aon_gpio1_cb(uint8_t group, uint8_t aIndex) {
//    uint8_t value;
//    APP_MSG_T comm_send_msg;
//	BaseType_t xHigherPriorityTaskWoken;
//	/* We have not woken a task at the start of the ISR. */
//    xHigherPriorityTaskWoken = pdFALSE;
//
//    hx_drv_gpio_get_in_value(AON_GPIO1, &value);
//    dbg_printf(DBG_LESS_INFO, "aon_gpio1_cb(%d, %d, %d)\n", group, aIndex, value);
//
//    comm_send_msg.msg_data = value;
//    comm_send_msg.msg_event = APP_MSG_COMMEVENT_AON_GPIO1_INT;
//    dbg_printf(DBG_LESS_INFO, "send to comm task 0x%x\r\n", comm_send_msg.msg_event);
//	xQueueSendFromISR( xCommTaskQueue, &comm_send_msg, &xHigherPriorityTaskWoken );
//	hx_drv_gpio_clr_int_status(AON_GPIO1);
//	if( xHigherPriorityTaskWoken )
//    {
//    	taskYIELD();
//    }
//}

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
static void aon_gpio0_interupt_init(void) {

#ifdef ORIGINAL_CODE
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
    hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
    hx_drv_gpio_cb_register(AON_GPIO0, aon_gpio0_cb);
    hx_drv_gpio_set_input(AON_GPIO0);
    hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
#else
    // I found this worked for Seeed board to WW130 interface

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

#endif	// ORIGINAL_CODE
}

// Not used
///**
// * Potentially initialises PA1 pin to cause an interrupt.
// * Not used at the time of writing
// *
// * This registers a callback to aon_gpio1_cb() which in turn sends an event to the comm_task loop,
// * which in turn sends a message to main_task
// *
// */
//static void aon_gpio1_interupt_init(void) {
//    hx_drv_scu_set_PA1_pinmux(SCU_PA1_PINMUX_AON_GPIO1, 1);
//    hx_drv_gpio_set_int_type(AON_GPIO1, GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
//    hx_drv_gpio_cb_register(AON_GPIO1, aon_gpio1_cb);
//    hx_drv_gpio_set_input(AON_GPIO1);
//    hx_drv_gpio_set_int_enable(AON_GPIO1, 1);
//
//}

/**
 * Switches PA0 from an interrupt input to an active low output.
 *
 * This will interrupt the WW130. Thie pin needs to be taken high agin
 * by aon_gpio0_drive_high() after a suitale delay.
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
 * In teh first part, aon_gpio0_drive_low() took the pin low.
 */
static void aon_gpio0_drive_high(void) {
	uint8_t pinValue;

	hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);

	// This for testing:
	hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);

	XP_LT_RED;
	xprintf("Set PA0 as an output, drive to 1 (AON_GPIO0). Read back as %d\n", pinValue);
	XP_WHITE;

	// Now set PA0 as an input and prepare it to respond to interrupts from teh WW130.
	hx_drv_gpio_set_input(AON_GPIO0);

	// The next commands prepare PA0 to be an interrupt input
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt
}


/**
 * FreeRTOS timer to release PA0
 *
 * We could just set the pin high at this point, but by sending an event to teh queue
 * we can keep a more consisent track of evts.
 */
static void pa0InterruptExpired(xTimerHandle pxTimer) {
    APP_MSG_T comm_send_msg;

    comm_send_msg.msg_data = 0;
    comm_send_msg.msg_event = APP_MSG_COMMEVENT_PA0_TIMER;

	if(xQueueSend( xCommTaskQueue , (void *) &comm_send_msg , __QueueSendTicksToWait) == pdTRUE) {
	    dbg_printf(DBG_LESS_INFO, "Sent to comm task 0x%x\r\n", comm_send_msg.msg_event);
	}
	else {
		dbg_printf(DBG_LESS_INFO, "send comm_send_msg=0x%x fail\r\n", comm_send_msg.msg_event);
	}
}

/*********************************** Other Local Function Definitions ************************************************/

///**
// * Utility to determine if we are in ISR context
// */
//static bool acutetech_utils_inISR(void) {
//	if ( SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk ) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}

/**
 * Initialises our I2C slave module
 *
 * Sets up our slave address and callback functions.
 *
 */
static void app_i2ccomm_init(void) {
    iic_id = USE_DW_IIC_SLV_0;

    hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg[iic_id]);

    i2ccomm_clear_read_buf_header();
    hx_lib_i2ccomm_start(iic_id, (unsigned char *)&gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);

    // I think we need to enable reads here:
    hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
}


/**
 * Clears I2C header bytes.
 * (sets 4 header bytes to 0xff)
 *
 * Called in preparation for receiving a command from the I2C master
 */
static void i2ccomm_clear_read_buf_header(void) {
    memset((void *)&gRead_buf[iic_id], 0xff, 4);
    hx_CleanDCache_by_Addr((void *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
}


/**
 * Command process for FEATURE:SYSTEM_INFO
 *
 * Parse I2C message when 1st byte of the incoming I2C master message is 0
 * Checks the checksum.
 * Extracts the 2nd byte (cmd)
 * For recognised commands, prepare a response to be read by an expected I2C master read.
 * Currently only 3 commands (in eventi2ccomm.h)
 	 	 	 I2CCOMM_CMD_SYS_GET_VER_BSP get version number of BSP
    		I2CCOMM_CMD_SYS_GET_VER_I2C get version number of I2C communication protocol
    		I2CCOMM_CMD_SYS_GET_VER_FORMAL get formal version number of BSP
 *
 * TODO - check how/when the read is called - hx_lib_i2ccomm_enable_read()
 */
void evt_i2cs_cmd_process_sysinfo(void) {
    int retval = 0;
    unsigned int data;
    unsigned char cmd;
    unsigned short checksum;

    // This is the Himax CRC16 check which uses an algorithm unknown to me (CGP)
    retval = hx_lib_i2ccomm_validate_checksum((unsigned char *) &gRead_buf[iic_id]);

    if (retval != I2CCOMM_NO_ERROR) {
        dbg_evt_iics_cmd("%s - checksum validation : FAIL\n", __FUNCTION__);
        i2ccomm_clear_read_buf_header();
        hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
        return;
    }

    cmd = gRead_buf[iic_id][I2CFMT_COMMAND_OFFSET];
    dbg_evt_iics_cmd("%s(iic_id:%d, cmd:0x%02x) \n", __FUNCTION__, iic_id, cmd);

    switch (cmd) {

        case I2CCOMM_CMD_SYS_GET_VER_BSP:
            // prepare write buffer for write process
            gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
            gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SYS_GET_VER_BSP;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP >> 8) & 0xFF;

            data = 0xA1B2C3D4;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET] = data & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 1] = (data >> DATA_SFT_OFFSET_8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 2] = (data >> DATA_SFT_OFFSET_16) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 3] = (data >> DATA_SFT_OFFSET_24) & 0xFF;

            hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP, &checksum);

            if (retval == I2CCOMM_NO_ERROR)  {
                dbg_evt_iics_cmd("checksum generation : 0x%04x \n", checksum);
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = checksum & 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else   {
                dbg_evt_iics_cmd("[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = 0xFF;
            }

            hx_CleanDCache_by_Addr((void *) &gWrite_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
            break;

        case I2CCOMM_CMD_SYS_GET_VER_I2C:
            // prepare write buffer for write process
            gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
            gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SYS_GET_VER_I2C;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C >> 8) & 0xFF;

            data = hx_lib_i2ccomm_version();
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET] = data & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 1] = (data >> DATA_SFT_OFFSET_8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 2] = (data >> DATA_SFT_OFFSET_16) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 3] = (data >> DATA_SFT_OFFSET_24) & 0xFF;
            retval = hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C, &checksum);

            if (retval == I2CCOMM_NO_ERROR)  {
                dbg_evt_iics_cmd("checksum generation : 0x%04x \n", checksum);
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = checksum & 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else  {
                dbg_evt_iics_cmd("[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET] = 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CFMT_PAYLOAD_OFFSET + 1] = 0xFF;
            }

            hx_CleanDCache_by_Addr((void *) &gWrite_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
            break;

        default:
            /* only get command without write data or information to external I2C master
               or  undefine command */
            i2ccomm_clear_read_buf_header();
            hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            break;
    }
}

/*********************************** Exported Function Definitions ************************************************/

/*!
 * FreeRTOS task responsible for task communication
 *
 * This handles events from GPIO pins (e.g. PA0)
 * and from I2C data RX and TX
 */
void comm_task(void *pvParameters) {
    APP_MSG_T comm_recv_msg;
    APP_MSG_T main_send_msg;
	APP_COMM_TASK_STATE_E old_state;

#ifdef CRCTEST
    // Test how the CRC generation and checking works
    check_crc();
#endif	// CRCTEST

    //init GPIO 0/1, if enable GPIO0/1, please make sure these GPIO are not floating
    aon_gpio0_interupt_init();
    //aon_gpio1_interupt_init(); // not used

    app_i2ccomm_init();

    // Initialise any inter-board communications and provide a callback for I2C messages
    ww130_cmd_init();

    // Create a timer that turns off the PA0 interrupt pulse
    timerHndlPa0 = xTimerCreate(
    		"timer1Sec", /* name */
			pdMS_TO_TICKS(PA0PULSEWIDTH), /* period/time */
			pdFALSE, /* NO auto reload */
			(void*)0, /* timer ID */
			pa0InterruptExpired); /* callback */

    if (timerHndlPa0 == NULL) {
    	// Error!
    	dbg_printf(DBG_LESS_INFO, "Failed to create timer");
    }
    else {
    	dbg_printf(DBG_LESS_INFO, "PA0 will pulse for %dms", PA0PULSEWIDTH);
    }

    g_commtask_state = APP_COMM_TASK_STATE_INIT;

    for (;;)  {
    	if (xQueueReceive ( xCommTaskQueue , &(comm_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE ) {
    		XP_BROWN
    		dbg_printf(DBG_LESS_INFO, "\ncomm_task");
    		XP_WHITE;
    		dbg_printf(DBG_LESS_INFO, " received message = 0x%x\r\n", comm_recv_msg.msg_event);

    		old_state = g_commtask_state;

    		switch(comm_recv_msg.msg_event)  {

    			case APP_MSG_COMMEVENT_AON_GPIO0_INT:
    				// Interrupt on PA0 pin from WW130 - send a message to main_task
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = comm_recv_msg.msg_data;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_AON_GPIO0_INT;
					if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
						dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
					}
    	   		break;

    			case APP_MSG_COMMEVENT_AON_GPIO1_INT:
    				// Potential interrupt from a GPIO pin - not used
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = comm_recv_msg.msg_data;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_AON_GPIO1_INT;
					if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
						dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
					}
    				break;

    			case APP_MSG_COMMEVENT_SB_GPIO0_INT:
    				// Potential interrupt from a GPIO pin - not used
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = 0;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_SB_GPIO0_INT;
    				if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    					dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    				}
                   break;

    			case APP_MSG_COMMEVENT_SB_GPIO1_INT:
    				// Potential interrupt from a GPIO pin - not used
    				g_commtask_state = APP_COMM_TASK_STATE_GPIO_INT;
    				main_send_msg.msg_data = 0;
    				main_send_msg.msg_event = APP_MSG_MAINEVENT_SB_GPIO1_INT;
    				if(xQueueSend( xMainTaskQueue , (void *) &main_send_msg , __QueueSendTicksToWait) != pdTRUE) {
    					dbg_printf(DBG_LESS_INFO, "send main_send_msg=0x%x fail\r\n", main_send_msg.msg_event);
    				}
    				break;

    			case APP_MSG_COMMEVENT_I2CCOMM_RX:
    				// I2C master has sent us data
    				evt_i2ccomm_0_rx_cb();
    				break;

    			case APP_MSG_COMMEVENT_I2CCOMM_TX:
    				// I2C master has read our data
    				evt_i2ccomm_0_tx_cb();
    				break;

    			case APP_MSG_COMMEVENT_I2CCOMM_ERR:
    				// I2C error
    				evt_i2ccomm_0_err_cb();
    				break;

    			case APP_MSG_COMMEVENT_SPISEND_PIC:
    				// Not currently used
    				dbg_printf(DBG_LESS_INFO, "APP_MSG_COMMEVENT_SPISEND_PIC\r\n");
    				break;

        	   	case APP_MSG_COMMEVENT_PA0_LOW:
        	   		// Request from software to interrupt the WW130
        	   		aon_gpio0_drive_low();
        	   		// Start a timer so that the pin is taken high again soon

    				dbg_printf(DBG_LESS_INFO, "DEBUG: change period to %d\r\n", comm_recv_msg.msg_data);

    				if( xTimerChangePeriod(timerHndlPa0, (comm_recv_msg.msg_data / portTICK_PERIOD_MS), 100 ) != pdPASS ) {
    					// failure
    				}

        	   		if (xTimerStart(timerHndlPa0, 0)!= pdPASS) {
        	   		  // failure
        	   		}
        	   		break;

        	   	case APP_MSG_COMMEVENT_PA0_TIMER:
        	   		// Request from timer to terminate the interrupt to the WW130
        	   		aon_gpio0_drive_high();
        	   		break;

    			default:
    				//TODO error
    				g_commtask_state = APP_COMM_TASK_STATE_ERROR;
    				break;
            }

    		if (old_state != g_commtask_state) {
    			// state has changed
        		XP_BROWN;
        		dbg_printf(DBG_LESS_INFO, "  comm_state ");
        		XP_WHITE;
        		dbg_printf(DBG_LESS_INFO, "changed from %d to %d\r\n", old_state, g_commtask_state);
    		}
        }
	}
}

/**
 * Client registers a callback
 *
 * When certain messages arrive from the I2C master (WW130) they are routed to ww130_cmd.c by this callback.
 *
 */
void comm_task_customer_register_cb(void *cb_func) {
    i2ccomm_cmd_customer_process = cb_func;
}

/**
 * Called by the client when it has a message to return to the I2C master.
 *
 * If a string is being sent, the trailing '\0' will have been added.
 *
 * Calls hx_lib_i2ccomm_enable_write() and response arrives in i2cs_cb_rx()
 *
 * @param message = pointer to the buffer contauning the message
 * @param messageType =
 * @param length = number of bytes to send
 */
void comm_task_customer_response(uint8_t * message, aiProcessor_msg_type_t messageType, uint16_t length) {
    uint16_t checksum;
    I2CCOMM_ERROR_E errcode;
    int32_t e_no;
    uint32_t payload_len;

    if (length > WW130_MAX_PAYLOAD_SIZE) {
    	length = WW130_MAX_PAYLOAD_SIZE;
    }

    // prepare write buffer for write process
    gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = WW130_CMD_FEATURE;
    gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = messageType; // master is receiving a string
    gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = length & 0xff;
    gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (length >> 8) & 0xff;

    // Now copy the payload to the right place
	memcpy(&gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET], message, length);

#ifdef USECRC16CCITT
	crc16_ccitt_generate(gWrite_buf[iic_id], I2CFMT_PAYLOAD_OFFSET + length, &checksum);
	// CRC is BE
    gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + length] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
    gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + length + 1] = checksum & 0xFF;
#else
    hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP, &checksum);
    // CRC is LE
    gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + length] = checksum & 0xFF;
    gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + length + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;

#endif	// USECRC16CCITT

    dbg_evt_iics_cmd("Sending %d payload bytes. Checksum generated: 0x%04x \n", length, checksum);

    hx_CleanDCache_by_Addr((void *) &gWrite_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);

    // for debugging, print the buffer
    printf_x_printBuffer((uint8_t *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + length + I2CCOMM_CHECKSUM_SIZE);

#if 0
    I2CCOMM_ERROR_E res;
	// response arrives in i2cs_cb_rx()
    res = hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
    if (res != 0) {
    	xprintf("I2C read erros %d\n", res);
    }
#else

    errcode = I2CCOMM_NO_ERROR;
    e_no = E_OK;

    if(iic_id >= DW_IIC_S_NUM){
    	xprintf("%s, iic_id(%d) is not support \n", __FUNCTION__, iic_id);
        return; // I2CCOMM_ERROR_INVALID_PARAMETERS;
    }

    if(gWrite_buf[iic_id] == NULL){
        return; // I2CCOMM_ERROR_BUFFER_NOT_ASSIGN;
    }

    payload_len = gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] + (gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] << 8);
    xprintf("Payload length = %d. Sending %d bytes in total.\n", payload_len, (I2CCOMM_HEADER_SIZE + payload_len + I2CCOMM_CHECKSUM_SIZE));

    //xprintf("%s(iic_id:%d) (s)\n", __FUNCTION__, iic_id);

    e_no = hx_drv_i2cs_interrupt_write(iic_id, EVT_I2CS_0_SLV_ADDR, gWrite_buf[iic_id],
    		(I2CCOMM_HEADER_SIZE + payload_len + I2CCOMM_CHECKSUM_SIZE), (void *)i2cs_cb_tx);

    if(e_no != E_OK){
        errcode = I2CCOMM_ERROR_DRVIER_WRITE_FAIL;
    	xprintf("I2C write error %d\n", errcode);
    }
    //xprintf("%s(iic_id:%d) (e):%d %d\n", __FUNCTION__, iic_id, e_no, errcode);

    //return errcode;

#endif
}
