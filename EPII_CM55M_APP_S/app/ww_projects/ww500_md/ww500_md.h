/*
 * ww500_md.h
 *
 *  Created on: Dec 3, 2020
 *      Author: 902447
 */

#ifndef WW500_MD_H_
#define WW500_MD_H_

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "WE2_device.h"
#include "WE2_core.h"
#include "board.h"
#include "if_task.h"

/*************************************** Definitions *******************************************/

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
    #define configCOMMAND_INT_MAX_OUTPUT_SIZE     WW130_MAX_PAYLOAD_SIZE
#endif

#define NUMBEROFTASKS	5

// for now, 10s
#define INACTIVITYTIMEOUT 10000
// For cold boot go straight to sleep
#define INACTIVITYTIMEOUTCB 1000

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


// Possible wakeup reasons
typedef enum {
	APP_WAKE_REASON_UNKNOWN,
	APP_WAKE_REASON_COLD,	// Cold boot
	APP_WAKE_REASON_MD,		// Motion detection
	APP_WAKE_REASON_BLE,	// BLE activity
	APP_WAKE_REASON_RADAR,	// Radar motion detection
	APP_WAKE_REASON_TIMER,	// RTC
} APP_WAKE_REASON_E;

int app_main(void);

// LED control
void app_ledGreen(bool on);
void app_ledBlue(bool on);

char * app_get_version_string(void);
char * app_get_board_name_string(void);


void app_onInactivityDetection(void);

#endif // WW500_MD_H_
