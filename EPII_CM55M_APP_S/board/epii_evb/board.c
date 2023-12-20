/*
 * board.c
 *
 *  Created on: 2021/02/03
 *      Author: 902447
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "WE2_device.h"
#ifdef LIB_COMMON
#include "console_io.h"
#include "xprintf.h"
#endif
#include "pinmux_init.h"
#include "platform_driver_init.h"
#ifdef IP_timer
#include "hx_drv_timer.h"
#endif
#if defined(FREERTOS_SECONLY) || defined(FREERTOS_NS)
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#if defined(FREERTOS_OSHAL) 
#include "cmsis_os2.h"
#endif
#endif
#if defined(RTE_CMSIS_RTOS2)
#if defined(RTE_CMSIS_RTOS2_FreeRTOS)
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "FreeRTOSConfig.h"
#endif
#if defined(RTE_CMSIS_RTOS2_RTX5)
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "RTX_Config.h"
#endif
#endif
#if defined(RTOS2_RTX_SECONLY) || defined(RTOS2_RTX_NS)
#include "cmsis_os2.h"
#include "RTX_Config.h"
#endif
#if defined(RTOS2_FREERTOS_SECONLY) || defined(RTOS2_FREERTOS_NS)
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#endif
#ifdef IP_timer
#include "timer_interface.h"
#endif

int board_init() {

	/*set pinmux init*/
	pinmux_init();
	/*platform driver init*/
	platform_driver_init();
#ifdef LIB_COMMON
	xprintf_setup();
#endif

	return 0;
}


int board_deinit()
{
#ifdef LIB_COMMON
	xprintf_desetup();
#endif

	/*TODO Driver deinit*/

	return 0;
}

int board_delay_ms(uint32_t ms)
{
#if !defined(RTE_CMSIS_RTOS2)
#if defined(FREERTOS_SECONLY) || defined(FREERTOS_NS)
#if !defined(FREERTOS_OSHAL) 
    vTaskDelay(pdMS_TO_TICKS(ms));
    return 0;
#else
    osDelay((ms * configTICK_RATE_HZ) / 1000);
    return 0;
#endif
#endif
#if defined(RTOS2_FREERTOS_SECONLY) || defined(RTOS2_FREERTOS_NS)
    osDelay((ms * configTICK_RATE_HZ) / 1000);
    return 0;
#endif
#if defined(RTOS2_RTX_SECONLY) || defined(RTOS2_RTX_NS)
    osDelay((ms * OS_TICK_FREQ) / 1000);
    return 0;
#endif
#elif defined(RTE_CMSIS_RTOS2)
#if defined(RTE_CMSIS_RTOS2_FreeRTOS)
	osDelay((ms * configTICK_RATE_HZ) / 1000);
	return 0;
#endif
#if defined(RTE_CMSIS_RTOS2_RTX5)
	osDelay((ms * OS_TICK_FREQ) / 1000);
	return 0;
#endif
#endif
#ifdef IP_timer
	if(hx_drv_timer_cm55x_delay_ms(ms, TIMER_STATE_DC) == TIMER_NO_ERROR)
	{
		return 0;
	}else{
		return -1;
	}
#endif
	return -1;
}
