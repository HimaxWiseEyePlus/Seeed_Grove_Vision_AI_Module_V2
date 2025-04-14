/*
 * inactivity.c
 *
 *  Created on: 13 Apr 2025
 *      Author: charl
 */

/********************************** Includes ******************************************/


#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#include "inactivity.h"

/**************************************** Local routine declarations  *************************************/

static void inactivity_timer_callback(TimerHandle_t xTimer);

/**************************************** Local Variables **************************************/

// Static state
static TimerHandle_t inactivity_timer = NULL;
static void (*inactivity_callback)(void) = NULL;
static TickType_t inactivity_timeout_ticks = 0;
static BaseType_t inactivity_enabled = pdFALSE;

// Idle hook state
static TickType_t idle_start_tick = 0;
static BaseType_t inactivity_triggered = pdFALSE;

/**************************************** Local function definitions  *************************************/

/**
 * This is the local callback that executes when the timer expires
 *
 * It calls the function that was registered in the initialisation process.
 *
 */
static void inactivity_timer_callback(TimerHandle_t xTimer) {
    if (inactivity_callback) {
        inactivity_callback();
    }
}

/**
 * Called when FreeRTOS enters the idle state.
 *
 * If used this must be defined in FreeRTOSConfig.h as follows:
 *
 * #define configUSE_IDLE_HOOK 1
 */
void vApplicationIdleHook(void) {
    TickType_t now;

    if (!inactivity_enabled || inactivity_timeout_ticks == 0) {
    	return;
    }

    now = xTaskGetTickCount();

    if (idle_start_tick == 0) {
        idle_start_tick = now;
    }

    if (!inactivity_triggered && (now - idle_start_tick >= inactivity_timeout_ticks)) {
        inactivity_triggered = pdTRUE;

        if (inactivity_callback) {
            inactivity_callback();
        }
    }
}


/**************************************** Global function definitions  *************************************/

/**
 * Initialise a one-shot timer and start it.
 *
 * @param timeout_ms the duration of the timer.
 * @param callback - the function to call when it expires.
 *
 */
void inactivity_init(uint32_t timeout_ms, void (*callback)(void)) {

    if (callback == NULL || timeout_ms == 0) {
        return; // Invalid input
    }

    inactivity_callback = callback;
    inactivity_timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    inactivity_triggered = pdFALSE;
    idle_start_tick = 0;
    inactivity_enabled = pdTRUE;

    if (inactivity_timer == NULL) {
        inactivity_timer = xTimerCreate("InactivityTimer",
                                        inactivity_timeout_ticks,
                                        pdFALSE,
                                        NULL,
                                        inactivity_timer_callback);
    }

    if (inactivity_timer != NULL) {
        xTimerStart(inactivity_timer, 0);
    }
}

/**
 * Function called by each task to indicate that they are active.
 */
void inactivity_reset(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!inactivity_enabled) {
    	return;
    }

    // Reset idle tracking
    idle_start_tick = 0;
    inactivity_triggered = pdFALSE;

    if (inactivity_timer == NULL) {
    	return;
    }

    if (xPortIsInsideInterrupt()) {
        xTimerResetFromISR(inactivity_timer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xTimerReset(inactivity_timer, 0);
    }
}


/**
 * Auto-reset idle tracking on any non-idle task switch-in
 *
 * If used this must be defined in FreeRTOSConfig.h as follows:
 *
 * #define traceTASK_SWITCHED_IN() inactivity_ on_task_switched_in()
 *
 */
void inactivity_on_task_switched_in(void) {
    if (!inactivity_enabled) {
    	return;
    }

    if (xTaskGetCurrentTaskHandle() != xTaskGetIdleTaskHandle()) {
        idle_start_tick = 0;
        inactivity_triggered = pdFALSE;
    }
}

