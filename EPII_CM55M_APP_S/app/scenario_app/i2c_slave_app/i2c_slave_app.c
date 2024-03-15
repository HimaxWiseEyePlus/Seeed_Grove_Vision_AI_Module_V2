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
#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "hx_drv_scu.h"
#include "hx_drv_timer.h"
#include "hx_drv_iic.h"

void i2cs_task();
void I2C_SLV_Init();
void I2C_SLV_DeInit();

/*!
 * @brief Main function
 */
int app_main(void) {

	xprintf("Start I2C Slave App\n");

	i2cs_task();

	while(1);

	return 0;
}


/******************************************************************************
 * I2C slave task
 ******************************************************************************/
#define I2CS_0_ID	0x62
#define BUFF_SIZE	128
static HX_DRV_DEV_IIC *dev_iic_slv;
static uint32_t slave_addr = I2CS_0_ID;
static uint8_t rx_buffer[BUFF_SIZE];
static uint8_t tx_buffer[BUFF_SIZE];
static uint8_t result[10][BUFF_SIZE];
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

static void i2c_s_callback_fun_tx(void* param)
{
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    xprintf("%s\n", __FUNCTION__);
	hx_drv_timer_cm55s_stop();
}

static void i2c_s_callback_fun_rx(void* param)
{
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

	xprintf("%s \n", __FUNCTION__);

    uint8_t feature = rx_buffer[0];
    uint8_t cmd     = rx_buffer[1];

    g_index = rx_buffer[2];

    xprintf("feature: %0x, cmd: %0x, index: %d\n", feature, cmd, g_index);

    switch (cmd) {
    case FEATURE_TRANSPORT_CMD_WRITE:
        xprintf("cmd: CMD_WRITE(%d)\n", g_index);
        break;

    case FEATURE_TRANSPORT_CMD_READ:
        xprintf("cmd: CMD_READ(%d)\n", g_index);
		xprintf("Prepare data, delay 30ms\n");
		EPII_cpu_nop_us(30000);
		memcpy(tx_buffer, result[g_index], BUFF_SIZE);
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
		xprintf("cmd: CMD_AVAILABLE\n");
        tx_buffer[0] = FEATURE_TRANSPORT_CMD_AVAILABLE;
        tx_buffer[1] = g_index;
        i2cs_write_enable(2);
        break;

    case FEATURE_TRANSPORT_CMD_RESET:
        xprintf("cmd: CMD_RESET\n");
        break;

    default:
        break;
    }
}


static void i2c_s_callback_fun_err(void* param)
{
    HX_DRV_DEV_IIC*      iic_obj      = (HX_DRV_DEV_IIC*)param;
    HX_DRV_DEV_IIC_INFO* iic_info_ptr = &(iic_obj->iic_info);

    xprintf("%s, err_code = %d\n", __FUNCTION__, iic_info_ptr->err_state);

	// i2c master read but there is no "CMD" form i2c master, return previous result to avoid clock stretching
	if ( iic_info_ptr->err_state == DEV_IIC_ERR_TX_DATA_UNREADY)
	{
		xprintf("\nDEV_IIC_ERR_TX_DATA_UNREADY, g_index = %d\n", g_index);
		memcpy(tx_buffer, result[g_index], BUFF_SIZE);
        i2cs_write_enable(8);
    }
}


void I2C_SLV_Init()
{
    uint32_t i2c_s_id = USE_DW_IIC_SLV_0;

    xprintf("I2C Slave 0 Init\n");

    hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_I2C_S_SCL_0, 1);
    hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_I2C_S_SDA_0, 1);

    hx_drv_i2cs_init(i2c_s_id, HX_I2C_HOST_SLV_0_BASE);

    dev_iic_slv = hx_drv_i2cs_get_dev(i2c_s_id);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_TXCB, (void *)i2c_s_callback_fun_tx);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_RXCB, (void *)i2c_s_callback_fun_rx);
    dev_iic_slv->iic_control(DW_IIC_CMD_SET_ERRCB, (void *)i2c_s_callback_fun_err);
	dev_iic_slv->iic_control(DW_IIC_CMD_SLV_SET_SLV_ADDR, (void*)slave_addr);

	i2cs_read_enable(BUFF_SIZE);

}


void I2C_SLV_DeInit()
{
    uint32_t i2c_s_id = USE_DW_IIC_SLV_0;

    hx_drv_i2cs_deinit(i2c_s_id);

}


void i2cs_read_enable(uint32_t size)
{
	uint32_t i2c_s_id = USE_DW_IIC_SLV_0;
    memset(rx_buffer, 0, BUFF_SIZE);
    hx_drv_i2cs_interrupt_read(i2c_s_id, slave_addr, rx_buffer, size, (void*)i2c_s_callback_fun_rx);
}


void i2cs_write_enable(uint32_t size)
{
	uint32_t i2c_s_id = USE_DW_IIC_SLV_0;
	hx_drv_i2cs_interrupt_write(i2c_s_id, slave_addr, tx_buffer, size, (void*)i2c_s_callback_fun_tx);
	//register a tx timeout callback to avoid i2c slave halted
	i2cs_tx_timer(I2CS_TX_TIME_OUT_MS);
}


void i2cs_task()
{
	//init result
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < BUFF_SIZE; j++)
			result[i][j] = i;

	I2C_SLV_Init();
}
