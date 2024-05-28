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
//#include "veneer_table.h"
//
#endif
#endif

#include "xprintf.h"
#include "timer_interface.h"
#include "hx_drv_scu.h"
#include "hx_drv_iic.h"
#include "imu_read_app.h"
#include "icm42688.h"

/*!
 * @brief Main function
 */
int app_main(void) {

	uint8_t wbuffer[6];
	uint8_t rbuffer[128];
	// uint8_t addr[2];
	// uint32_t i, data_size = 6, r_size = 32;
	IIC_ERR_CODE_E i2c_err;
	icm42688_axis_t accel, gyro;

	xprintf("Start IMU Read App\n");

	// The output pin of I2C Master 0 is defined by the user application.
	hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_I2C_M_SCL, 1);
	hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_I2C_M_SDA, 1);

	// initializes the I2C Master 0 with SCL speed of 400 KHz
	hx_drv_i2cm_init(USE_DW_IIC_0, HX_I2C_HOST_MST_0_BASE, DW_IIC_SPEED_FAST);
	hx_drv_i2cm_set_err_cb(USE_DW_IIC_0, i2cm_0_err_cb);

	if ( icm42688_init() != IIC_ERR_OK )
	{
		xprintf("icm42688_init() fail!\n");
		return -1;
	}

	while ( 1 )
	{
		i2c_err = icm42688_get_accel_axis(&accel);
		if ( i2c_err != 0 )
			xprintf("icm42688_get_accel_axis ERROR(%d)\n", i2c_err);

		i2c_err = icm42688_get_gyro_axis(&gyro);
		if ( i2c_err != 0 )
			xprintf("icm42688_get_gyro_axis ERROR(%d)\n", i2c_err);

		xprintf("accel x = 0x%x(%d)\n", accel.x, accel.x);
		xprintf("accel y = 0x%x(%d)\n", accel.y, accel.y);
		xprintf("accel z = 0x%x(%d)\n", accel.z, accel.z);
		xprintf("gyro  x = 0x%x(%d)\n", gyro.x, gyro.x);
		xprintf("gyro  y = 0x%x(%d)\n", gyro.y, gyro.y);
		xprintf("gyro  z = 0x%x(%d)\n", gyro.z, gyro.y);
		xprintf("----------------------------------------\n");

		i2c_err = icm42688_get_data(&accel, &gyro);
		if ( i2c_err != 0 )
			xprintf("icm42688_get_data ERROR(%d)\n", i2c_err);

		xprintf("FIFO accel x = 0x%x(%d)\n", accel.x, accel.x);
		xprintf("FIFO accel y = 0x%x(%d)\n", accel.y, accel.y);
		xprintf("FIFO accel z = 0x%x(%d)\n", accel.z, accel.z);
		xprintf("FIFO gyro  x = 0x%x(%d)\n", gyro.x, gyro.x);
		xprintf("FIFO gyro  y = 0x%x(%d)\n", gyro.y, gyro.y);
		xprintf("FIFO gyro  z = 0x%x(%d)\n", gyro.z, gyro.y);
		xprintf("----------------------------------------\n");

		hx_drv_timer_cm55x_delay_ms(100, TIMER_STATE_DC);
	}
	return 0;
}


volatile uint32_t g_err_cb = 0;

void i2cm_0_err_cb(void *status)
{
    HX_DRV_DEV_IIC *iic_obj = status;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

	g_err_cb = 1;
	xprintf("[%s] err:%d \n", __FUNCTION__, iic_info_ptr->err_state);
}
