/*
 * inactivity.h
 *
 * Functions to support detection of inactivity in all tasks.
 *
 *  Created on: 13 Apr 2025
 *      Author: charl
 */

#ifndef INACTIVITY_H_
#define INACTIVITY_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the inactivity timer system.
 *
 * @param timeout_ms Timeout period in milliseconds after which the inactivity callback is triggered.
 * @param callback Function to be called when inactivity timeout is reached.
 */
void inactivity_init(uint32_t timeout_ms, void (*callback)(void));

/**
 * @brief Resets the inactivity timer to indicate activity.
 *        Safe to call from both task and ISR contexts.
 */
void inactivity_reset(void);
/*
*
 * @brief This function must be implemented for FreeRTOS idle hook.

void vApplicationIdleHook(void);*/

/**
 * @brief Optional: Use this with traceTASK_SWITCHED_IN() to auto-reset on task activity.
 */
void inactivity_on_task_switched_in(void);

#ifdef __cplusplus
}
#endif


#endif /* INACTIVITY_H_ */
