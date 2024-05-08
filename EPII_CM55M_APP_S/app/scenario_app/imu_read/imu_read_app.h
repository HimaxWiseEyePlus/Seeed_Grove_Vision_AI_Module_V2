/*
 * imu_read_app.h
 *
 *  Created on: Apr 30, 2024
 *      Author: Steve
 */

#ifndef APP_SCENARIO_IMU_READ_APP_
#define APP_SCENARIO_IMU_READ_APP_

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

int app_main(void);
void i2cm_0_err_cb(void *status);

#endif /* APP_SCENARIO_IMU_READ_APP_ */
