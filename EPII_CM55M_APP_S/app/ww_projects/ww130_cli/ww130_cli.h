/*
 * ww130_cli.h
 *
 *  Created on: Dec 3, 2020
 *      Author: 902447
 */

#ifndef APP_WW_PROJECTS_WW_TEMPLATE_H_
#define APP_WW_PROJECTS_WW_TEMPLATE_H_

#include <stdio.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"


#include "FreeRTOS.h"
#include "task.h"


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


#ifndef configCOMMAND_INT_MAX_OUTPUT_SIZE
// Otherwise in FreeRTOSConfig.h (e.g. as 2048)
    #define configCOMMAND_INT_MAX_OUTPUT_SIZE     256
#endif

#define NUMBEROFTASKS	4

// Define function pointer types
typedef uint16_t (*int_func_ptr)(void);
typedef const char* (*str_func_ptr)(void);

// Structure to allow retrieval of internal state
typedef struct {
	TaskHandle_t	task_id;
	int_func_ptr	getState;
	str_func_ptr 	stateString;
	uint16_t		priority;
} internal_state_t;

int app_main(void);

#endif // APP_WW_PROJECTS_WW_TEMPLATE_H_
