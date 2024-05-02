/*
 * hello_world.h
 *
 *  Created on: Dec 3, 2020
 *      Author: 902447
 */

#ifndef SCENARIO_ALLON_SENSOR_TFLM_FREERTOS_H_
#define SCENARIO_ALLON_SENSOR_TFLM_FREERTOS_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"

#if defined(FREERTOS_SECONLY) || \
    defined(FREERTOS_NS) || \
    defined(RTE_CMSIS_RTOS2_FreeRTOS) || \
    defined(RTOS2_FREERTOS_SECONLY) || \
    defined(RTOS2_FREERTOS_NS)
#define __MsToTicks(ms)         (((ms) * configTICK_RATE_HZ) / 1000)
#endif
#if defined(RTE_CMSIS_RTOS2_RTX5) || \
    defined(RTOS2_RTX_SECONLY) || \
    defined(RTOS2_RTX_NS)
#define __MsToTicks(ms)         (((ms) * OS_TICK_FREQ) / 1000)
#endif

#define __QueueSendTicksToWait  __MsToTicks(1000)
#define __QueueRecvTicksToWait  (portMAX_DELAY)

int app_main(void);

#define APP_BLOCK_FUNC() do{ \
	__asm volatile("b    .");\
	}while(0)

typedef enum
{
	APP_STATE_ALLON,
    APP_STATE_RESTART,
    APP_STATE_STOP,
}APP_STATE_E;

#endif /* SCENARIO_ALLON_SENSOR_TFLM_FREERTOS_H_ */
