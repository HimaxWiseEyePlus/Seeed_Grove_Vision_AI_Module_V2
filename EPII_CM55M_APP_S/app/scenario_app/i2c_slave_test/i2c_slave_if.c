/*
 * File: i2c_slave_if.c
 *
 *  Created on: 11/07/2024
 *      Author: charl
 *
 * This contains functions that support inter-baord communication
 *
 * Note that is we decide to use FreeRTOS, then the project 'allon_sensor_fflm_freertos'
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
#include "WE2_core.h"
#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "hx_drv_scu.h"
#include "hx_drv_timer.h"
#include "hx_drv_iic.h"
#include "i2c_slave_if.h"

#include "hx_drv_gpio.h"


/********************************* Local Defines ******************************/
// TODO is this necessary?
#define I2CS_TX_TIME_OUT_MS				1000

#define NUM_PARAMS	3

// Time in us
// TODO work out what this should be
#define PULSELOWTIME	100	//

// Slave ID address. Must be the same as the WW130 uses
#define I2CS_0_ID	0x62

/********************************* Local Function Declarations ********************/

static void i2cs_cb_tx(void *param);
static void i2cs_cb_rx(void *param);
static void i2cs_cb_err(void *param);
static void i2cs_tx_timeout_cb(void *data);

static void i2cs_write_enable(uint32_t size);
static void i2cs_read_enable(uint32_t size);
static void i2cs_tx_timer(uint32_t timeout_ms);

static void aon_gpio0_cb(uint8_t group, uint8_t aIndex);

// Callback function prototype
typedef void (*CallbackFunction)(uint8_t cmd, uint8_t *message, uint16_t length);

/********************************* Local Variables *********************************/

static uint32_t i2c_s_id;

static HX_DRV_DEV_IIC *dev_iic_slv;
static uint32_t slave_addr;
static uint8_t rx_buffer[I2C_SLAVE_IF_BUFF_SIZE + NUM_PARAMS];	// allow
static uint8_t tx_buffer[I2C_SLAVE_IF_BUFF_SIZE];
static uint8_t result[10][I2C_SLAVE_IF_BUFF_SIZE];
static uint32_t g_index = 0;

static CallbackFunction clientCallback;

/********************************* Local Function Definitions ********************/

static void i2cs_tx_timeout_cb(void *data) {
	// if i2c slave tx complete timeout, reset i2c slave
    xprintf("\n\n#### [%s], i2cs_reset ####\n\n", __FUNCTION__);
    hx_drv_i2cs_reset(i2c_s_id);

    i2c_slave_if_init(clientCallback);
    i2c_slave_if_deinit();
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
 */
static void  i2cs_cb_tx(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    uint32_t length = iic_info_ptr->tx_buf.len;
    uint32_t offset = iic_info_ptr->tx_buf.ofs;
	xprintf("[%s] Sent %d bytes from a buffer of %d\n", __FUNCTION__,
			(uint16_t) offset, (uint16_t) length);

	hx_drv_timer_cm55s_stop();
}


/**
 * This function is called whenever the I2C slave is written to by the master.
 * Received data is available in rx_buffer[]
 *
 * I have made the size of rx_buffer larger by the 3 expected parameter bytes
 */
static void  i2cs_cb_rx(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    uint8_t cmd;
    uint8_t param1;
    uint8_t param2;
    uint32_t length;
    uint32_t offset;

    length = iic_info_ptr->rx_buf.len;
    offset = iic_info_ptr->rx_buf.ofs;

    // TODO remind myself what length and offset are...
	xprintf("[%s] RX buffer size: %d. Received %d bytes\n", __FUNCTION__,
			(uint16_t) length, (uint16_t) offset);

	if (length < NUM_PARAMS) {
		// malformed message
		xprintf("Mal-formed message - too short\n");
		return;
	}

	// Parameter bytes arrive first
    cmd = rx_buffer[0];
    param1 = rx_buffer[1];
    param2 = rx_buffer[2];

    xprintf("   Parameters: cmd: %d, param 1: %d, param 2: %d\n", cmd, param1, param2);

    // For now send every message.
    clientCallback(cmd, &rx_buffer[NUM_PARAMS], length - NUM_PARAMS);

//    switch (cmd) {
//    case AI_PROCESSOR_MSG_TX_STRING:
//    	break;
//
//    case AI_PROCESSOR_MSG_TX_BINARY:
//
//    	break;
//
//    case AI_PROCESSOR_MSG_RX_STRING:
//
//    	break;
//
//    case AI_PROCESSOR_MSG_RX_BINARY:
//
//    	break;
//
//    default:
//        xprintf("Unrecognised command %0x\n", cmd);
//    	break;
//    }
}

/**
 * This is the I2C error callback.
 */
static void  i2cs_cb_err(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    xprintf("[%s], err_code = %d\n", __FUNCTION__, iic_info_ptr->err_state);

	// i2c master read but there is no "CMD" from i2c master,
    // return previous result to avoid clock stretching
	if ( iic_info_ptr->err_state == DEV_IIC_ERR_TX_DATA_UNREADY)
	{
		xprintf("\nDEV_IIC_ERR_TX_DATA_UNREADY, g_index = %d\n", g_index);
		memcpy(tx_buffer, result[g_index], I2C_SLAVE_IF_BUFF_SIZE);
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
void i2c_slave_if_init(void * callback) {
	uint8_t gpio_value;



	clientCallback = callback;


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

	// Prepare for incoming I2C message
	i2cs_read_enable(I2C_SLAVE_IF_BUFF_SIZE + NUM_PARAMS);
}

/**
 * Uninitilise I2C slave
 */
void i2c_slave_if_deinit(void) {
    uint32_t i2c_s_id = USE_DW_IIC_SLV_0;

    hx_drv_i2cs_deinit(i2c_s_id);
}


/**
 * Request to send this via I2C
 * Assumes the message has already been set up in tx_buffer
 */
void i2cs_slave_if_send_string(char * message) {
	uint16_t length = strnlen(message, I2C_SLAVE_IF_BUFF_SIZE);

	// TODO - prepend any message type bytes?
	memcpy(tx_buffer, message, length);
	i2cs_write_enable(length);
	xprintf("DEBUG: preparing to send %d byte response: '%s'", length, tx_buffer);
}

// Toggle inter-processor interrupt pin to wake WW130
void i2cs_slave_wake_master(void) {
	// Disable interrupt so we don't interrut oursleves
	hx_drv_gpio_set_int_enable(AON_GPIO0, 0);	// 0 means disable interrupt

	// Sets PA0 as an output and drive low, then delay, then high, then set as an input
	hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);
	EPII_cpu_nop_us(PULSELOWTIME);	// wait a bit

	hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);
	hx_drv_gpio_set_input(AON_GPIO0);

	// The next commands prepare PA0 to be an interrupt input
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt
}
