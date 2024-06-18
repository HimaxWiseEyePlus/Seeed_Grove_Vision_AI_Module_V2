/*
 * hello_world.h
 *
 *  Created on: Dec 3, 2020
 *      Author: 902447
 */

#ifndef APP_SCENARIO_ALLON_SENSOR_TFLM_
#define APP_SCENARIO_ALLON_SENSOR_TFLM_

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef enum
{
	APP_STATE_ALLON,
}APP_STATE_E;

int app_main(void);

#endif /* APP_SCENARIO_ALLON_SENSOR_TFLM_ */
