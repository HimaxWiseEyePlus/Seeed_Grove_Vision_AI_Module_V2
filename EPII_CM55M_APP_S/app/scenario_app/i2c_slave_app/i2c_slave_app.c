/**
 * i2c_slave_app.c
 *
 * Example emailed by Himax as an example of using the I2C as a slave.
 *
 * Heavily modified by CGP to demonstrate I2C and interrupt operations
 * with WW130.B00
 *
 * For operation of this program with the WW130 see the file 'operation.txt'
 * in the WW130 project 'twi_master_seeed'
 *
 */

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

void i2cs_task();
void I2C_SLV_Init();
void I2C_SLV_DeInit();
void i2cs_write_enable(uint32_t size);
void i2cs_read_enable(uint32_t size);

// CGP Some changes when this flag is defined:
#define CGP_MODS

#ifdef CGP_MODS
#include "hx_drv_gpio.h"
#endif	// CGP_MODS


/*!
 * @brief Main function
 */
int app_main(void) {

	xprintf("\n\n *** Start I2C Slave App Example.    Built: %s %s ***\n", __DATE__, __TIME__);
	xprintf("For operation of this program with the WW130 see the file 'operation.txt'\n");
	xprintf("in the WW130 project 'twi_master_seeed'\n");
	 i2cs_task();

	while(1);

	return 0;
}


/******************************************************************************
 * I2C slave task
 ******************************************************************************/
#define I2CS_0_ID	0x62
static HX_DRV_DEV_IIC *dev_iic_slv;
static uint32_t slave_addr = I2CS_0_ID;

#ifdef CGP_MODS
#define I2C_SLAVE_IF_BUFF_SIZE	48
#define NUM_PARAMS	3
// Time in us
#define PULSELOWTIME	100000	// 100ms
//#define PUSHPULL				// PA0 operates as push-pull (always driven, to 0 or 1)
static uint8_t rx_buffer[I2C_SLAVE_IF_BUFF_SIZE + NUM_PARAMS];	// allow
static uint8_t tx_buffer[I2C_SLAVE_IF_BUFF_SIZE];
static uint8_t result[10][I2C_SLAVE_IF_BUFF_SIZE];
#else
#define I2C_SLAVE_IF_BUFF_SIZE	128
static uint8_t rx_buffer[I2C_SLAVE_IF_BUFF_SIZE];
static uint8_t tx_buffer[I2C_SLAVE_IF_BUFF_SIZE];
static uint8_t result[10][I2C_SLAVE_IF_BUFF_SIZE];
#endif	// CGP_MODS

static uint32_t g_index = 0;

#define FEATURE_TRANSPORT               0x10
#define FEATURE_TRANSPORT_CMD_STATUS    0x00
#define FEATURE_TRANSPORT_CMD_READ      0x01
#define FEATURE_TRANSPORT_CMD_WRITE     0x02
#define FEATURE_TRANSPORT_CMD_AVAILABLE 0x03
#define FEATURE_TRANSPORT_CMD_START     0x04
#define FEATURE_TRANSPORT_CMD_STOP      0x05
#define FEATURE_TRANSPORT_CMD_RESET     0x06

#define I2CS_TX_TIME_OUT_MS				1000

void i2cs_tx_timeout_cb(void *data)
{
	// if i2c slave tx complete timeout, reset i2c slave
    xprintf("\n\n#### [%s], i2cs_reset ####\n\n", __FUNCTION__);
    hx_drv_i2cs_reset(USE_DW_IIC_SLV_0);
	I2C_SLV_DeInit();
	I2C_SLV_Init();
}

/**
 * Prepares a timer that can expire if the master does not read
 * the slave's data within a time period.
 *
 * The timer callback is defined here.
 */
void i2cs_tx_timer(uint32_t timeout_ms)
{
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
static void i2c_s_callback_fun_tx(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    uint32_t length = iic_info_ptr->tx_buf.len;
    uint32_t offset = iic_info_ptr->tx_buf.ofs;
	xprintf("[%s] Sent %d bytes from a buffer of %d\n", __FUNCTION__,
			(uint16_t) offset, (uint16_t) length);

	hx_drv_timer_cm55s_stop();
}

/**
 * Routine executed when the master sends data to the slave.
 *
 * This is the original functionality as provided by Himax.
 *
 * I assume this is derived from some project where this functionality makes sense,
 * but is seems pretty random to me.
 *
 * This code executes this if the first byte ("feature") == 0.
 */
static void oldFunctions(uint8_t cmd) {

	switch (cmd) {
	case FEATURE_TRANSPORT_CMD_WRITE:
		// cmd = 2 Sets the value of g_index
		xprintf("cmd: CMD_WRITE(%d)\n", g_index);
		break;

	case FEATURE_TRANSPORT_CMD_READ:
		// cmd = 1. Copies result[] to tx_buffer[] then sends 8 bytes of tx_buffer
		// Imposes a 30ms delay befor the data is available for the master to read
		xprintf("cmd: CMD_READ(%d)\n", g_index);
		xprintf("Prepare data, delay 30ms\n");
		EPII_cpu_nop_us(30000);
		memcpy(tx_buffer, result[g_index], I2C_SLAVE_IF_BUFF_SIZE);
#if 0
		xprintf("\nresult[%d] = ", g_index);
		for (int i = 0; i < 10; i++)
		{
			xprintf("0x%x ", result[g_index][i]);
		}
#endif
		xprintf("\ntx_buffer = ");
		for (int i = 0; i < 10; i++)
		{
			xprintf("0x%x ", tx_buffer[i]);
		}
		xprintf("\n");

		i2cs_write_enable(8);
		break;

	case FEATURE_TRANSPORT_CMD_AVAILABLE:
		// cmd = 3. Returns two bytes
		xprintf("cmd: CMD_AVAILABLE\n");
		tx_buffer[0] = FEATURE_TRANSPORT_CMD_AVAILABLE;
		tx_buffer[1] = g_index;
		i2cs_write_enable(2);
		break;

	case FEATURE_TRANSPORT_CMD_RESET:
		// cmd = 6. Does nothing
		xprintf("cmd: CMD_RESET\n");
		break;

	default:
		break;
	}
}

/**
 * Routine executed when the master sends data to the slave.
 *
 * This is new functionality as designed by CGP
 *
 * This modified code executes this if the first byte ("feature") == 1.
 */
static void newFunctions(uint8_t cmd, uint16_t offset) {
	static uint16_t statusCount = 0;
	uint8_t pinValue;

	switch (cmd) {

	case FEATURE_TRANSPORT_CMD_STATUS:
		// cmd = 0. Does nothing sensible
		xprintf("TBD - Some status\n");
		tx_buffer[0] = FEATURE_TRANSPORT_CMD_AVAILABLE;
		tx_buffer[1] = statusCount++;
		tx_buffer[2] = g_index;
		i2cs_write_enable(3);	// prepare to send 3 bytes to the master
		break;


	case FEATURE_TRANSPORT_CMD_WRITE:
		// cmd = 2. Master writes data
		// I assume that up to BUFF_SIZE bytes have arrived.
		// Copy these to the selected result[] array, start after the parameters
		memcpy(result[g_index], &rx_buffer[NUM_PARAMS], offset);
		xprintf("Data written to buffer %d (%d bytes)\n", g_index, offset - NUM_PARAMS);
		break;

	case FEATURE_TRANSPORT_CMD_READ:
		// cmd = 1. Master reads data
		// Copy the contents of the selected result[] array to tx_buffer
		memcpy(tx_buffer, result[g_index], I2C_SLAVE_IF_BUFF_SIZE);
		xprintf("Reading data from buffer %d (up to %d bytes)\n", g_index, I2C_SLAVE_IF_BUFF_SIZE);

		i2cs_write_enable(I2C_SLAVE_IF_BUFF_SIZE);	// prepare to send a full buffer to the master

		break;

	case FEATURE_TRANSPORT_CMD_AVAILABLE:
		// cmd = 3. Redefined as a command for this board to pulse the PA0 pin low, then return to being an input.
#ifdef PUSHPULL
		hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_LOW);
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 (AON_GPIO0) low. Read back as %d\n", pinValue);
		EPII_cpu_nop_us(PULSELOWTIME);	// wait a bit
		hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 (AON_GPIO0) high. Read back as %d\n", pinValue);
#else
		// disable the interrupt, so we don't interrupt ourself
		hx_drv_gpio_set_int_enable(AON_GPIO0, 0);	// 0 means disable interrupt

		// Sets PA0 as an output and drive low, then delay, then high, then set as an input
	    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);
		EPII_cpu_nop_us(PULSELOWTIME);	// wait a bit
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 as an output, drive to 0 (AON_GPIO0). Read back as %d\n", pinValue);
		hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 as an output, drive to 1 (AON_GPIO0). Read back as %d\n", pinValue);
		hx_drv_gpio_set_input(AON_GPIO0);

		// The next commands prepare PA0 to be an interrupt input
		hx_drv_gpio_clr_int_status(AON_GPIO0);
		hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt

#endif	// PUSHPULL
		break;

	case FEATURE_TRANSPORT_CMD_START:
		// cmd = 4. Make PA0 high.
		// This pin connects to WW130 /AIINT and SW2, so it can cause an interrupt etc.
#ifdef PUSHPULL
		// Sets PA0 high (actively drive high)
		hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 (AON_GPIO0). Read back as %d\n", pinValue);
#else
		// Sets PA) as an input. Pull-up should take it to logic 1.
	    hx_drv_gpio_set_input(AON_GPIO0);
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 as an input (AON_GPIO0). Read back as %d\n", pinValue);

		// The next commands prepare PA0 to be an interrupt input
		hx_drv_gpio_clr_int_status(AON_GPIO0);
		hx_drv_gpio_set_int_enable(AON_GPIO0, 1);	// 1 means enable interrupt

#endif	// PUSHPULL

		break;

	case FEATURE_TRANSPORT_CMD_STOP:
		// cmd = 5. Sets PA0 low.
#ifdef PUSHPULL
		// Sets PA0 low (actively drive low)
		hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_LOW);
		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 (AON_GPIO0). Read back as %d\n", pinValue);
#else
		// disable the interrupt, so we don't interrupt ourself
		hx_drv_gpio_set_int_enable(AON_GPIO0, 0);	// 0 means disable interrupt

		// Sets PA0 as an output and drive low.
	    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_LOW);
		//hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_LOW);

		hx_drv_gpio_get_in_value(AON_GPIO0, &pinValue);
		xprintf("Set PA0 as an output, drive to 0 (AON_GPIO0). Read back as %d\n", pinValue);
#endif	// PUSHPULL

		break;

	case FEATURE_TRANSPORT_CMD_RESET:
		// cmd = 6. Does nothing
		xprintf("TBD - 'reset'\n");
		g_index = 0;
		break;

	default:
		xprintf("TBD - unexpected command %d\n", cmd);
		break;
	}
}

/**
 * This function is called whenever the I2C slave is written to by the master.
 * Received data is available in rx_buffer[]
 *
 * I have made the size of rx_buffer larger by the 3 expected parameter bytes
 */
static void i2c_s_callback_fun_rx(void* param) {
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    uint32_t length = iic_info_ptr->rx_buf.len;
    uint32_t offset = iic_info_ptr->rx_buf.ofs;
	xprintf("[%s] RX buffer size: %d. Received %d bytes\n", __FUNCTION__,
			(uint16_t) length, (uint16_t) offset);

	// Parameter bytes arrive first
    uint8_t feature = rx_buffer[0];
    uint8_t cmd     = rx_buffer[1];
    g_index 		= rx_buffer[2];

    xprintf("   Parameters: feature: %0x, cmd: %0x, index: %d\n", feature, cmd, g_index);

    // CGP - I decided to redefine the operation. If feature == 0 use the old code.
    // If feature == 1 use the new code.
    switch (feature) {
    case 0:
    	oldFunctions(cmd);
    	break;

    case 1:
    	newFunctions(cmd, offset);
    	break;

    default:
        xprintf("No operation defined for feature: %0x\n", feature);
    	break;

    }
}

/**
 * This is the I2C error callback.
 */
static void i2c_s_callback_fun_err(void* param) {
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
 * Initialise the I2C slave peripheral.
 *
 * SDA = PA3 pin
 * SCL = PA2 pin
 *
 * Defines callbacks for data TX and RX, and error.
 * Defines the I2C slave address
 *
 */
void I2C_SLV_Init()
{
    uint32_t i2c_s_id = USE_DW_IIC_SLV_0;

    xprintf("I2C Slave 0 Init: Slave address = 0x%02x\n", slave_addr);

    hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_I2C_S_SCL_0, 1);
    hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_I2C_S_SDA_0, 1);

    hx_drv_i2cs_init(i2c_s_id, HX_I2C_HOST_SLV_0_BASE);

    dev_iic_slv = hx_drv_i2cs_get_dev(i2c_s_id);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_TXCB, (void *)i2c_s_callback_fun_tx);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_RXCB, (void *)i2c_s_callback_fun_rx);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_ERRCB, (void *)i2c_s_callback_fun_err);
	dev_iic_slv->iic_control(DW_IIC_CMD_SLV_SET_SLV_ADDR, (void*)slave_addr);

	// Prepare for incoming I2C messages
#ifdef CGP_MODS
	i2cs_read_enable(I2C_SLAVE_IF_BUFF_SIZE + NUM_PARAMS);
#else
	i2cs_read_enable(I2C_SLAVE_IF_BUFF_SIZE);
#endif	// CGP_MODS
}

/**
 * Uninitilise I2C slave
 */
void I2C_SLV_DeInit() {
    uint32_t i2c_s_id = USE_DW_IIC_SLV_0;

    hx_drv_i2cs_deinit(i2c_s_id);
}

/**
 * Initialise I2C slave reading.
 *
 * It looks like this only needs to be called once.
 * size defines the maximum number of bytes to be read i.e. the max number of bytes the I2C master should write.
 */
void i2cs_read_enable(uint32_t size)
{
	uint32_t i2c_s_id = USE_DW_IIC_SLV_0;
#ifdef CGP_MODS
	memset(rx_buffer, 0, sizeof(rx_buffer));
#else
	memset(rx_buffer, 0, I2C_SLAVE_IF_BUFF_SIZE);
#endif	// CGP_MODS

    hx_drv_i2cs_interrupt_read(i2c_s_id, slave_addr, rx_buffer, size, (void*)i2c_s_callback_fun_rx);
}

/**
 * Prepare the I2C slave to send data to the master.
 *
 * A timer is started. If the master does not read the data before the timer expires,
 * a timer error is triggered.
 *
 * Prepares a buffer of suitable size for the expected outgoing data.
 */
void i2cs_write_enable(uint32_t size) {
	uint32_t i2c_s_id = USE_DW_IIC_SLV_0;
	hx_drv_i2cs_interrupt_write(i2c_s_id, slave_addr, tx_buffer, size, (void*)i2c_s_callback_fun_tx);
	//register a tx timeout callback to avoid i2c slave halted
	i2cs_tx_timer(I2CS_TX_TIME_OUT_MS);
}

#if defined (CGP_MODS) && !defined (PUSHPULL)

/**
 * Interrupt service routine for PA0
 */
void aon_gpio0_cb(uint8_t group, uint8_t aIndex) {
    uint8_t gpio_value;

    hx_drv_gpio_get_in_value(AON_GPIO0, &gpio_value);
    xprintf("PA0 Interrupt Pin is %d. (Group: %d, aIndex: %d)\n", gpio_value, group, aIndex);

	hx_drv_gpio_clr_int_status(AON_GPIO0);
}
#endif	// defined (CGP_MODS) && !defined (PUSHPULL)

/**
 * Initialisation
 *
 * Called from main() which then loops forever.
 */
void i2cs_task() {

#ifdef CGP_MODS
	uint8_t gpio_value;

	//init result
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < I2C_SLAVE_IF_BUFF_SIZE; j++) {
			// A more interesting pattern!
			result[i][j] = (i << 4) + (j & 0x1f);
		}
	}

#ifdef PUSHPULL
	// When PUSHPULL is defined the PA0 is always an output to the WW130 - driven as 0 or 1
	// Initialise PA0 as an active high output. This device can then set PA0 to 0 or 1
    hx_drv_gpio_set_output(AON_GPIO0, GPIO_OUT_HIGH);
    // For reasons I don't understand, we must use SCU_PA0_PINMUX_AON_GPIO0_2 not SCU_PA0_PINMUX_AON_GPIO0_0
    //hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_2, 1);
	hx_drv_gpio_set_out_value(AON_GPIO0, GPIO_OUT_HIGH);

	hx_drv_gpio_get_in_value(AON_GPIO0, &gpio_value);
	xprintf("Initialise PA0 (AON_GPIO0) as push-pull output, high. Read as %d\n", gpio_value);
#else
	// When PUSHPULL is not defined the PA0 can either be an output to the WW130 - driven as 0 - or an input.
	// When PA0 is an input then it can be enabled as an interrupt. This allows the WW130 to interrupt this board by driving PA0 low.
	// That is - the same PA0 signal can be used by either side to interrupt the other.

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
#endif	// PUSHPULL

#else
	//init result
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < I2C_SLAVE_IF_BUFF_SIZE; j++) {
			result[i][j] = i;
		}
	}
#endif	// CGP_MODS
	I2C_SLV_Init();
}


